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
#include <vtkMRMLLinearTransformNode.h>
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
#include "imstkMeshToMeshBruteforceCD.h"
#include "imstkPBDCollisionHandling.h"
#include "imstkDummyClient.h"
#include "imstkSceneObjectController.h"



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
  //this->GetSDK()->addScene(scene);
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
  double dt)
{
  this->AddImmovableObject(
    this->GetSDK()->getScene(name),
    modelNode,
    dt
  );
}

//----------------------------------------------------------------------------
void vtkSlicerCollisionSimulationLogic::AddImmovableObject(
  vtkMRMLModelNode* modelNode,
  double dt)
{
  this->AddImmovableObject(
    this->GetSDK()->getActiveScene(),
    modelNode,
    dt
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
  double dt)
{
  if (!scene
    || !modelNode)
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

  auto volTetMesh = std::dynamic_pointer_cast<imstk::TetrahedralMesh>(mesh);
  volTetMesh->scale(1.0, imstk::Geometry::TransformType::ApplyToData);
  auto surfMesh = std::make_shared<imstk::SurfaceMesh>();
  volTetMesh->extractSurfaceMesh(surfMesh, true);

  auto material = std::make_shared<imstk::RenderMaterial>();
  material->setDisplayMode(imstk::RenderMaterial::DisplayMode::WIREFRAME_SURFACE);
  auto surfMeshModel = std::make_shared<imstk::VisualModel>(surfMesh);
  surfMeshModel->setRenderMaterial(material);


  auto P2CMap = std::make_shared<imstk::OneToOneMap>();
  P2CMap->setMaster(surfMesh);
  P2CMap->setSlave(surfMesh);
  P2CMap->compute();

  auto P2VMap = std::make_shared<imstk::OneToOneMap>();
  P2VMap->setMaster(surfMesh);
  P2VMap->setSlave(surfMesh);
  P2VMap->compute();

  auto C2VMap = std::make_shared<imstk::OneToOneMap>();
  C2VMap->setMaster(surfMesh);
  C2VMap->setSlave(surfMesh);
  C2VMap->compute();

  auto obj = std::make_shared<imstk::PbdObject>(modelNode->GetName());
  auto c_obj = std::make_shared<imstk::CollidingObject>(modelNode->GetName());
  obj->setCollidingGeometry(surfMesh);
  obj->addVisualModel(surfMeshModel);
  obj->setPhysicsGeometry(surfMesh);
  obj->setPhysicsToCollidingMap(P2CMap);
  obj->setPhysicsToVisualMap(P2VMap);
  obj->setCollidingToVisualMap(C2VMap);

  c_obj->setCollidingGeometry(surfMesh);
  c_obj->setVisualGeometry(surfMesh);
  c_obj->setCollidingToVisualMap(C2VMap);

  auto pbdFloorModel = std::make_shared<imstk::PbdModel>();
  pbdFloorModel->setModelGeometry(surfMesh);

  auto pbdParams2 = std::make_shared<imstk::PBDModelConfig>();
  pbdParams2->m_uniformMassValue = 0.0;
  pbdParams2->m_proximity = 0.1;
  pbdParams2->m_contactStiffness = 1.0;
  //pbdParams2->m_dt = dt;


  pbdFloorModel->configure(pbdParams2);
  obj->setDynamicalModel(pbdFloorModel);

  auto pbdSolverfloor = std::make_shared<imstk::PbdSolver>();
  pbdSolverfloor->setPbdObject(obj);

  scene->addNonlinearSolver(pbdSolverfloor);
  scene->addSceneObject(obj);
  this->m_Objects[modelNode->GetName()] = obj;
  this->m_Meshes[modelNode->GetName()] = surfMesh;
  this->m_Solvers[modelNode->GetName()] = pbdSolverfloor;
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

  auto mesh1 = this->m_Meshes[obj1];
  auto mesh2 = this->m_Meshes[obj2];
 
  auto solver = this->m_Solvers[obj1];
  //// Collisions
  auto colData = std::make_shared<imstk::CollisionData>();
  auto CD = std::make_shared<imstk::MeshToMeshBruteForceCD>(mesh1, mesh2, colData);

  auto CH = std::make_shared<imstk::PBDCollisionHandling>(imstk::CollisionHandling::Side::AB,
    CD->getCollisionData(), pbdObj1, pbdObj2, solver); //fix this!!!!
  scene->getCollisionGraph()->addInteractionPair(pbdObj1, pbdObj2, CD, CH, nullptr);

}

//----------------------------------------------------------------------------
void vtkSlicerCollisionSimulationLogic::AddDeformableObject(
  const std::string& name,
  vtkMRMLModelNode* modelNode,
  double gravity, double stiffness, double dt, double youngs, double poisson)
{
  this->AddDeformableObject(
    this->GetSDK()->getScene(name),
    modelNode,
    gravity,
    stiffness,
    dt,
    youngs,
    poisson
  );
}

//----------------------------------------------------------------------------
void vtkSlicerCollisionSimulationLogic::AddDeformableObject(
  vtkMRMLModelNode* modelNode,
  double gravity, double stiffness, double dt, double youngs, double poisson)
{
  this->AddDeformableObject(
    this->GetSDK()->getActiveScene(),
    modelNode,
    gravity,
    stiffness,
    dt,
    youngs,
    poisson
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
  double gravity, double stiffness, double dt, double youngs, double poisson)
{
  if (!scene
    || !modelNode)
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

  auto material = std::make_shared<imstk::RenderMaterial>();
  material->setDisplayMode(imstk::RenderMaterial::DisplayMode::WIREFRAME_SURFACE);
  auto surfMeshModel = std::make_shared<imstk::VisualModel>(surfMesh);
  surfMeshModel->setRenderMaterial(material);


  auto P2CMap = std::make_shared<imstk::OneToOneMap>();
  P2CMap->setMaster(mesh);
  P2CMap->setSlave(surfMesh);
  P2CMap->compute();

  auto P2VMap = std::make_shared<imstk::OneToOneMap>();
  P2VMap->setMaster(mesh);
  P2VMap->setSlave(surfMesh);
  P2VMap->compute();

  auto C2VMap = std::make_shared<imstk::OneToOneMap>();
  C2VMap->setMaster(surfMesh);
  C2VMap->setSlave(surfMesh);
  C2VMap->compute();

  auto obj = std::make_shared<imstk::PbdObject>(modelNode->GetName());
  obj->setCollidingGeometry(surfMesh);
  obj->addVisualModel(surfMeshModel);
  obj->setPhysicsGeometry(volTetMesh);
  obj->setPhysicsToCollidingMap(P2CMap);
  obj->setPhysicsToVisualMap(P2VMap);
  obj->setCollidingToVisualMap(C2VMap);

  auto pbdFloorModel = std::make_shared<imstk::PbdModel>();
  pbdFloorModel->setModelGeometry(mesh);

  auto pbdParams = std::make_shared<imstk::PBDModelConfig>();

  // FEM constraint
  pbdParams->m_YoungModulus = youngs;
  pbdParams->m_PoissonRatio = poisson;
  pbdParams->enableFEMConstraint(imstk::PbdConstraint::Type::FEMTet, imstk::PbdFEMConstraint::MaterialType::NeoHookean);

  // Other parameters
  pbdParams->m_uniformMassValue = 1.0;
  pbdParams->m_gravity = imstk::Vec3d(0, 0, -50*gravity);
  pbdParams->m_dt = dt;
  pbdParams->m_maxIter = 2;
  pbdParams->m_proximity = 0.1;
  pbdParams->m_contactStiffness = stiffness;


  pbdFloorModel->configure(pbdParams);
  obj->setDynamicalModel(pbdFloorModel);

  auto pbdSolverfloor = std::make_shared<imstk::PbdSolver>();
  pbdSolverfloor->setPbdObject(obj);

  scene->addNonlinearSolver(pbdSolverfloor);
  scene->addSceneObject(obj);
  this->m_Objects[modelNode->GetName()] = obj;
  this->m_Meshes[modelNode->GetName()] = surfMesh;
  this->m_Solvers[modelNode->GetName()] = pbdSolverfloor;
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
  int wasModifying = modelNode->StartModify();
  this->UpdateMeshPointsFromObject(objectName, mesh);
  modelNode->SetAndObserveMesh(mesh);
  modelNode->EndModify(wasModifying);
}

void vtkSlicerCollisionSimulationLogic::AttachTransformController(const std::string & objectName, vtkMRMLLinearTransformNode * transform)
{
  auto scene = this->SDK->getActiveScene();
  if (!scene)
  {
    std::cout << "No scene" << std::endl;

    return;
  }

  this->AttachTransformController(scene->getName(), objectName, transform);
}

void vtkSlicerCollisionSimulationLogic::AttachTransformController(const std::string& name, const std::string & objectName, vtkMRMLLinearTransformNode * transform)
{
  if (!transform || objectName.empty())
  {
    std::cout << "No transform" << std::endl;

    return;
  }

  auto scene = this->SDK->getScene(name);
  if (!scene)
  {
    std::cout << "No scene" << std::endl;

    return;
  }

  auto object =
    std::dynamic_pointer_cast<imstk::PbdObject>(scene->getSceneObject(objectName));

  auto client_name = objectName + "_Controller";

  auto client = std::make_shared<imstk::DummyClient>(client_name);

  auto trackCtrl = std::make_shared<imstk::DeviceTracker>(client);
  //trackCtrl->setTranslationScaling(0.1);
  auto controller = std::make_shared<imstk::SceneObjectController>(object, trackCtrl);
  scene->addObjectController(controller);
  std::cout << "Controller added" << std::endl;
  this->m_TransformClients[transform->GetName()] = client;
}

void vtkSlicerCollisionSimulationLogic::UpdateControllerFromTransform(vtkMRMLLinearTransformNode * transform)
{
  if (!transform)
  {
    return;
    std::cout << "No transform" << std::endl;
  }
  
  auto scene = this->SDK->getActiveScene();
  if (!scene)
  {
    return;
    std::cout << "No scene" << std::endl;

  }

  auto client =
    this->m_TransformClients[transform->GetName()];

  //client->s

  if (!client)
  {
    return;
    std::cout << "No client" << std::endl;

  }

  vtkNew<vtkMatrix4x4> matrix;
  transform->GetMatrixTransformToParent(matrix.GetPointer());
  imstk::Vec3d p;
  p[0] = matrix->GetElement(0, 3);
  p[1] = matrix->GetElement(1, 3);
  p[2] = matrix->GetElement(2, 3);
  client->setPosition(p);

  Eigen::Matrix3d orient = (Eigen::Affine3d(Eigen::Matrix4d(matrix->GetData()))).rotation();
  imstk::Quatd qor(orient);
  client->setOrientation(qor);


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

  imstk::StdVectorOfVec3d newPoints = model->getCurrentState()->getPositions();

  vtkPoints* dataPoints = mesh->GetPoints();
  for (int i = 0; i < dataPoints->GetNumberOfPoints(); i++)
  {
    dataPoints->SetPoint(i, newPoints[i][0], newPoints[i][1], newPoints[i][2]);
  }
  mesh->SetPoints(dataPoints);
  mesh->Modified();
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
