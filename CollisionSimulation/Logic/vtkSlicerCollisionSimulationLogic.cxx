/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// CollisionSimulation Logic includes
#include "vtkSlicerCollisionSimulationLogic.h"

// MRML includes
#include <vtkMRMLCommandLineModuleNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLModelStorageNode.h>
#include <vtkMRMLStorableNode.h>
#include <vtkMRMLStorageNode.h>

// iMSTK includes
#include <imstkMeshIO.h>
#include <imstkOneToOneMap.h>
#include <imstkPbdModel.h>
#include <imstkPbdObject.h>
#include <imstkPbdSolver.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkUnstructuredGrid.h>

// ITK includes
#include <itksys/SystemTools.hxx>

// STD includes
#include <cassert>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerCollisionSimulationLogic);

//----------------------------------------------------------------------------
vtkSlicerCollisionSimulationLogic::vtkSlicerCollisionSimulationLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerCollisionSimulationLogic::~vtkSlicerCollisionSimulationLogic()
{
}

//----------------------------------------------------------------------------
std::string vtkSlicerCollisionSimulationLogic
::WriteNodeToTemporaryDirectory(vtkMRMLNode* node)
{
  vtkMRMLStorableNode* withStorageNode = vtkMRMLStorableNode::SafeDownCast(node);
  if (!withStorageNode)
    {
    return std::string();
    }

  vtkMRMLStorageNode* storageNode = withStorageNode->GetStorageNode();
  if (!storageNode)
    {
    withStorageNode->AddDefaultStorageNode();
    storageNode = withStorageNode->GetStorageNode();
    }

  std::string oldFilename = storageNode->GetFileName() ? storageNode->GetFileName() : "";
  std::string temporaryFilename = this->GetApplicationLogic()->GetTemporaryPath();
  temporaryFilename += "/";
  temporaryFilename += node->GetName();
  temporaryFilename += ".";
  temporaryFilename += storageNode->GetDefaultWriteFileExtension();
  storageNode->SetFileName(temporaryFilename.c_str());
  if (storageNode->WriteData(withStorageNode) != 1)
    {
    std::cerr << "Error when writing node to temporary path" << std::endl;
    temporaryFilename = "";
    }
  storageNode->SetFileName(oldFilename.c_str());
  return temporaryFilename;
}

//----------------------------------------------------------------------------
void vtkSlicerCollisionSimulationLogic
::CreateSDK(bool enableVR, bool disableRendering)
{
  if (!this->SDK)
  {
    this->SDK = std::make_shared<imstk::SimulationManager>(
      /*disableRendering=*/disableRendering,
      /*enableVR=*/enableVR
    );
  }
}

//----------------------------------------------------------------------------
std::shared_ptr<imstk::SimulationManager> vtkSlicerCollisionSimulationLogic
::GetSDK()
{
  if (!this->SDK)
  {
    this->CreateSDK();
  }
  return this->SDK;
}

//----------------------------------------------------------------------------
void vtkSlicerCollisionSimulationLogic
::CreateScene(const std::string& name, bool makeActive)
{
  auto scene = this->GetSDK()->createNewScene(name);
  this->GetSDK()->addScene(scene);
  if (makeActive)
  {
    this->SetActiveScene(name);
  }
}

//----------------------------------------------------------------------------
void vtkSlicerCollisionSimulationLogic
::SetActiveScene(const std::string& name, bool unloadPrevious)
{
  this->GetSDK()->setActiveScene(name, unloadPrevious);
}

//----------------------------------------------------------------------------
void vtkSlicerCollisionSimulationLogic::AddCollisionInteraction(
  const std::string& name,
  const std::string& obj1,
  const std::string& obj2)
{
  this->AddCollisionInteraction(
    this->GetSDK()->getScene(name), obj1, obj2);
}

//----------------------------------------------------------------------------
void vtkSlicerCollisionSimulationLogic::AddCollisionInteraction(
  const std::string& obj1,
  const std::string& obj2)
{
  this->AddCollisionInteraction(
    this->GetSDK()->getActiveScene(), obj1, obj2);
}

//----------------------------------------------------------------------------
void vtkSlicerCollisionSimulationLogic::AddImmovableObject(
  const std::string& name,
  vtkMRMLModelNode* modelNode,
  vtkMRMLCommandLineModuleNode* parameterNode)
{
  this->AddImmovableObject(
    this->GetSDK()->getScene(name),
    modelNode,
    parameterNode
  );
}

//----------------------------------------------------------------------------
void vtkSlicerCollisionSimulationLogic::AddImmovableObject(
  vtkMRMLModelNode* modelNode,
  vtkMRMLCommandLineModuleNode* parameterNode)
{
  this->AddImmovableObject(
    this->GetSDK()->getActiveScene(),
    modelNode,
    parameterNode
  );
}

//----------------------------------------------------------------------------
vtkMRMLCommandLineModuleNode* vtkSlicerCollisionSimulationLogic
::DefaultImmovableObjectParameterNode()
{
  vtkMRMLCommandLineModuleNode* parameterNode =
    vtkMRMLCommandLineModuleNode::New();
  parameterNode->SetParameterAsDouble("Number of constraints", 0);
  parameterNode->SetParameterAsDouble("Mass", 0.0);
  parameterNode->SetParameterAsDouble("Proximity", 0.1);
  parameterNode->SetParameterAsDouble("Contact stiffness", 1.0);
  return parameterNode;
}

//----------------------------------------------------------------------------
void vtkSlicerCollisionSimulationLogic::AddImmovableObject(
  std::shared_ptr<imstk::Scene> scene,
  vtkMRMLModelNode* modelNode,
  vtkMRMLCommandLineModuleNode* parameterNode)
{
  if (!scene
    || !modelNode
    || !parameterNode)
  {
    std::cerr << "Cannot add immovable object - wrong input" << std::endl;
    return;
  }

  std::string meshFilename = this->WriteNodeToTemporaryDirectory(modelNode);
  if (meshFilename.empty())
  {
    std::cerr << "Cannot write mesh node to temporary folder." << std::endl;
    return;
  }

  auto mesh = imstk::MeshIO::read(meshFilename);
  bool removed = itksys::SystemTools::RemoveFile(meshFilename.c_str());
  if (!removed)
  {
    vtkWarningMacro("Unable to delete temporary file");
  }

  auto oneToOneFloorNodalMap = std::make_shared<imstk::OneToOneMap>();
  oneToOneFloorNodalMap->setMaster(mesh);
  oneToOneFloorNodalMap->setSlave(mesh);
  oneToOneFloorNodalMap->compute();

  auto obj = std::make_shared<imstk::PbdObject>(modelNode->GetName());
  obj->setCollidingGeometry(mesh);
  obj->setVisualGeometry(mesh);
  obj->setPhysicsGeometry(mesh);
  obj->setPhysicsToCollidingMap(oneToOneFloorNodalMap);
  obj->setPhysicsToVisualMap(oneToOneFloorNodalMap);
  obj->setCollidingToVisualMap(oneToOneFloorNodalMap);

  auto pbdFloorModel = std::make_shared<imstk::PbdModel>();
  pbdFloorModel->setModelGeometry(mesh);

  double constraints = atof(parameterNode->GetParameterAsString("Number of constraints").c_str());
  double mass = atof(parameterNode->GetParameterAsString("Mass").c_str());
  double proximity = atof(parameterNode->GetParameterAsString("Proximity").c_str());
  double stiffness = atof(parameterNode->GetParameterAsString("Contact stiffness").c_str());

  pbdFloorModel->configure(/*Number of Constraints*/ constraints,
    /*Mass*/ mass,
    /*Proximity*/ proximity,
    /*Contact stiffness*/ stiffness
  );
  obj->setDynamicalModel(pbdFloorModel);

  auto pbdSolverfloor = std::make_shared<imstk::PbdSolver>();
  pbdSolverfloor->setPbdObject(obj);

  scene->addNonlinearSolver(pbdSolverfloor);
  scene->addSceneObject(obj);
}

//----------------------------------------------------------------------------
void vtkSlicerCollisionSimulationLogic::AddCollisionInteraction(
  std::shared_ptr<imstk::Scene> scene,
  const std::string& obj1,
  const std::string& obj2)
{
  if (!scene)
  {
    return;
  }
  auto pbdObj1 =
    std::dynamic_pointer_cast<imstk::PbdObject>(scene->getSceneObject(obj1));
  auto pbdObj2 =
    std::dynamic_pointer_cast<imstk::PbdObject>(scene->getSceneObject(obj2));
  if (!pbdObj1 || !pbdObj2)
  {
    return;
  }

  // Collisions
  auto colGraph = scene->getCollisionGraph();
  auto pair = std::make_shared<imstk::PbdInteractionPair>(
    imstk::PbdInteractionPair(pbdObj1, pbdObj2));
  pair->setNumberOfInterations(2);

  colGraph->addInteractionPair(pair);
}

//----------------------------------------------------------------------------
void vtkSlicerCollisionSimulationLogic::AddDeformableObject(
  const std::string& name,
  vtkMRMLModelNode* modelNode,
  vtkMRMLCommandLineModuleNode* parameterNode)
{
  this->AddDeformableObject(
    this->GetSDK()->getScene(name),
    modelNode,
    parameterNode
  );
}

//----------------------------------------------------------------------------
void vtkSlicerCollisionSimulationLogic::AddDeformableObject(
  vtkMRMLModelNode* modelNode,
  vtkMRMLCommandLineModuleNode* parameterNode)
{
  this->AddDeformableObject(
    this->GetSDK()->getActiveScene(),
    modelNode,
    parameterNode
  );
}

//----------------------------------------------------------------------------
vtkMRMLCommandLineModuleNode* vtkSlicerCollisionSimulationLogic
::DefaultDeformableObjectParameterNode()
{
  vtkMRMLCommandLineModuleNode* parameterNode =
    vtkMRMLCommandLineModuleNode::New();
  parameterNode->SetParameterAsDouble("Number of constraints", 1);
  parameterNode->SetParameterAsString("Constraint configuration", "FEM NeoHookean 1.0 0.3");
  parameterNode->SetParameterAsDouble("Mass", 1.0);
  parameterNode->SetParameterAsString("Gravity", "0 0 -9.8");
  parameterNode->SetParameterAsDouble("Time step", 0.001);
  parameterNode->SetParameterAsString("Fixed point", "");
  parameterNode->SetParameterAsDouble("Number of iteration in constraint solver", 2);
  parameterNode->SetParameterAsDouble("Proximity", 0.1);
  parameterNode->SetParameterAsDouble("Contact stiffness", 0.01);
  return parameterNode;
}

//----------------------------------------------------------------------------
void vtkSlicerCollisionSimulationLogic::AddDeformableObject(
  std::shared_ptr<imstk::Scene> scene,
  vtkMRMLModelNode* modelNode,
  vtkMRMLCommandLineModuleNode* parameterNode)
{
  if (!scene
    || !modelNode
    || !parameterNode)
  {
    std::cerr << "Cannot add deformable object - wrong input" << std::endl;
    return;
  }

  std::string meshFilename = this->WriteNodeToTemporaryDirectory(modelNode);
  if (meshFilename.empty())
  {
    std::cerr << "Cannot write mesh node to temporary folder." << std::endl;
    return;
  }

  auto mesh = imstk::MeshIO::read(meshFilename);
  bool removed = itksys::SystemTools::RemoveFile(meshFilename.c_str());
  if (!removed)
  {
    vtkWarningMacro("Unable to delete temporary file");
  }

  // Extract the surface mesh from the tetrahedral mesh
  auto volTetMesh = std::dynamic_pointer_cast<imstk::TetrahedralMesh>(mesh);
  volTetMesh->scale(1.0, imstk::Geometry::TransformType::ApplyToData);
  auto surfMesh = std::make_shared<imstk::SurfaceMesh>();
  volTetMesh->extractSurfaceMesh(surfMesh, true);

  auto oneToOneFloorNodalMap = std::make_shared<imstk::OneToOneMap>();
  oneToOneFloorNodalMap->setMaster(mesh);
  oneToOneFloorNodalMap->setSlave(mesh);
  oneToOneFloorNodalMap->compute();

  auto obj = std::make_shared<imstk::PbdObject>(modelNode->GetName());
  obj->setCollidingGeometry(mesh);
  obj->setVisualGeometry(mesh);
  obj->setPhysicsGeometry(mesh);
  obj->setPhysicsToCollidingMap(oneToOneFloorNodalMap);
  obj->setPhysicsToVisualMap(oneToOneFloorNodalMap);
  obj->setCollidingToVisualMap(oneToOneFloorNodalMap);

  auto pbdFloorModel = std::make_shared<imstk::PbdModel>();
  pbdFloorModel->setModelGeometry(mesh);

  double constraints = atof(parameterNode->GetParameterAsString("Number of constraints").c_str());
  std::string config = parameterNode->GetParameterAsString("Constraint configuration");
  double mass = atof(parameterNode->GetParameterAsString("Mass").c_str());
  std::string gravity = parameterNode->GetParameterAsString("Gravity");
  double timestep = atof(parameterNode->GetParameterAsString("Time Step").c_str());
  std::string fixedPoint = parameterNode->GetParameterAsString("Fixed point");
  double solverConstraints = atof(parameterNode->GetParameterAsString("Number of iteration in constraint solver").c_str());
  double proximity = atof(parameterNode->GetParameterAsString("Proximity").c_str());
  double stiffness = atof(parameterNode->GetParameterAsString("Contact stiffness").c_str());

  pbdFloorModel->configure(
    /*Number of Constraints*/ constraints,
    /*Constraint configuration*/ config,
    /*Mass*/ mass,
    /*Gravity*/ gravity,
    /*TimeStep*/ timestep,
    /*FixedPoint*/ fixedPoint,
    /*NumberOfIterationInConstraintSolver*/ solverConstraints,
    /*Proximity*/ proximity,
    /*Contact stiffness*/ stiffness
  );
  obj->setDynamicalModel(pbdFloorModel);

  auto pbdSolverfloor = std::make_shared<imstk::PbdSolver>();
  pbdSolverfloor->setPbdObject(obj);

  scene->addNonlinearSolver(pbdSolverfloor);
  scene->addSceneObject(obj);
}

//----------------------------------------------------------------------------
void vtkSlicerCollisionSimulationLogic
::UpdateMeshPointsFromObject(const std::string& objectName, vtkMRMLModelNode* modelNode)
{
  vtkUnstructuredGrid* mesh = modelNode ? modelNode->GetUnstructuredGrid() : nullptr;
  if (!mesh)
  {
    return;
  }
  this->UpdateMeshPointsFromObject(objectName, mesh);
  modelNode->SetAndObserveMesh(mesh);
}

//----------------------------------------------------------------------------
void vtkSlicerCollisionSimulationLogic
::UpdateMeshPointsFromObject(const std::string& objectName, vtkUnstructuredGrid* mesh)
{
  if (!mesh || objectName.empty())
  {
    return;
  }

  auto scene = this->SDK->getActiveScene();
  if (!scene)
  {
    return;
  }

  auto object =
    std::dynamic_pointer_cast<imstk::PbdObject>(scene->getSceneObject(objectName));

  if (!object)
  {
    auto objects = scene->getSceneObjects();
    for (int i = 0; i < objects.size(); ++i)
    {
      std::cout << objects[i]->getName() << std::endl;
    }

    return;
  }

  auto model =
    std::dynamic_pointer_cast<imstk::PbdModel>(object->getDynamicalModel());
  if (!model)
  {
    return;
  }

  imstk::StdVectorOfVec3d newPoints = model->getModelGeometry()->getVertexPositions();

  vtkPoints* dataPoints = mesh->GetPoints();
  for (int i = 0; i < dataPoints->GetNumberOfPoints(); i++)
  {
    dataPoints->SetPoint(i, newPoints[i][0], newPoints[i][1], newPoints[i][2]);
  }
}

//----------------------------------------------------------------------------
void vtkSlicerCollisionSimulationLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "SDK: " << this->SDK ? this->SDK : nullptr;
  if (this->SDK)
  {
    auto scene = this->SDK->getActiveScene();
    os << indent << "ActiveScene: " << scene ? scene->getName() : "No scene";
  }
}
