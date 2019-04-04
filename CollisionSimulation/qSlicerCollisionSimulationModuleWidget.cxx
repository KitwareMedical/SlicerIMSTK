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

// Qt includes
#include <QDebug>
#include <QTimer>

// SlicerQt includes
#include "vtkSlicerCollisionSimulationLogic.h"
#include "qSlicerCollisionSimulationModuleWidget.h"
#include "ui_qSlicerCollisionSimulationModuleWidget.h"

// Slicer includes
#include <vtkMRMLDisplayNode.h>
#include <vtkMRMLModelNode.h>

// VTK includes
#include <vtkUnstructuredGrid.h>

// iMSTK includes
#include <imstkDeformableObject.h>
#include <imstkMeshIO.h>
#include <imstkOneToOneMap.h>
#include <imstkPbdModel.h>
#include <imstkPbdObject.h>
#include <imstkPbdSolver.h>
#include <imstkScene.h>
#include <imstkSimulationManager.h>
#include <imstkSurfaceMesh.h>
#include <imstkTetrahedralMesh.h>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerCollisionSimulationModuleWidgetPrivate: public Ui_qSlicerCollisionSimulationModuleWidget
{
  Q_DECLARE_PUBLIC(qSlicerCollisionSimulationModuleWidget);
protected:
  qSlicerCollisionSimulationModuleWidget* const q_ptr;
public:
  qSlicerCollisionSimulationModuleWidgetPrivate(qSlicerCollisionSimulationModuleWidget& object);

  void setupSimulation();
  void setupOutput();

  QTimer MeshUpdateTimer;

  vtkMRMLModelNode* InputMeshNode;
  vtkMRMLModelNode* FloorMeshNode;
  vtkMRMLModelNode* OutputMeshNode;

  std::shared_ptr<imstk::Scene> Scene;
  std::shared_ptr<imstk::SimulationManager> SDK;

  std::shared_ptr<imstk::PbdObject> DeformableObject;
  std::shared_ptr<imstk::PbdModel> DeformableModel;

  // \todo Expose these ?
  const double scalingFactor = 1.;
  const double geoScalingFactor = 1.;
  const double solverTolerance = 1.0e-6;
  const double forceScalingFactor = 2.2 * 1.0e-1;
  const double timeStep = 0.04;
  const double materialPointSize = 4.0;
  const double materialLineWidth = 2.0;
};

//-----------------------------------------------------------------------------
// qSlicerCollisionSimulationModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerCollisionSimulationModuleWidgetPrivate
::qSlicerCollisionSimulationModuleWidgetPrivate(qSlicerCollisionSimulationModuleWidget& obj)
  : q_ptr(&obj)
{
  this->InputMeshNode = nullptr;
  this->FloorMeshNode = nullptr;
  this->OutputMeshNode = nullptr;
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidgetPrivate::setupSimulation()
{
  Q_Q(qSlicerCollisionSimulationModuleWidget);
  // Do you need to have two models ? One to follow and one to deform ?

  // Create a new scene
  this->Scene = this->SDK->createNewScene();

  // Get the mesh from its filename
  std::string meshFilename = q->simulationLogic()->ForceGetNodeFileName(this->InputMeshNode);
  auto mesh = imstk::MeshIO::read(meshFilename);

  std::string floorMeshFilename =
    q->simulationLogic()->ForceGetNodeFileName(this->FloorMeshNode);
  auto floorMesh = imstk::MeshIO::read(floorMeshFilename);

  // Extract the surface mesh from the tetrahedral mesh
  auto volTetMesh = std::dynamic_pointer_cast<imstk::TetrahedralMesh>(mesh);
  std::cout << "volTetMesh: " << volTetMesh << std::endl;
  volTetMesh->scale(this->geoScalingFactor, imstk::Geometry::TransformType::ApplyToData);
  auto surfMesh = std::make_shared<imstk::SurfaceMesh>();
  volTetMesh->extractSurfaceMesh(surfMesh, true);

  // Construct one to one nodal map based on the above meshes
  auto oneToOneNodalMap = std::make_shared<imstk::OneToOneMap>();
  oneToOneNodalMap->setMaster(volTetMesh);
  oneToOneNodalMap->setSlave(surfMesh);
  oneToOneNodalMap->compute();

  // Scene Object
  this->DeformableObject = std::make_shared<imstk::PbdObject>("DeformableObject");
  this->DeformableObject->setCollidingGeometry(surfMesh);
  this->DeformableObject->setVisualGeometry(surfMesh);
  this->DeformableObject->setPhysicsGeometry(volTetMesh);
  this->DeformableObject->setPhysicsToCollidingMap(oneToOneNodalMap);
  this->DeformableObject->setPhysicsToVisualMap(oneToOneNodalMap);
  this->DeformableObject->setCollidingToVisualMap(oneToOneNodalMap);

  // Create model
  this->DeformableModel = std::make_shared<imstk::PbdModel>();
  this->DeformableModel->setModelGeometry(volTetMesh);
  this->DeformableModel->configure( //\todo Make this configurable
    /*Number of Constraints*/ 1,
    /*Constraint configuration*/ "FEM NeoHookean 1.0 0.3",
    /*Mass*/ 1.0,
    /*Gravity*/ "0 0 -9.8",
    /*TimeStep*/ 0.001,
    /*FixedPoint*/ "",
    /*NumberOfIterationInConstraintSolver*/ 2,
    /*Proximity*/ 0.1,
    /*Contact stiffness*/ 0.01
  );
  this->DeformableObject->setDynamicalModel(this->DeformableModel);

  // Create solver
  auto pbdSolver = std::make_shared<imstk::PbdSolver>();
  pbdSolver->setPbdObject(this->DeformableObject);

  this->Scene->addNonlinearSolver(pbdSolver);
  this->Scene->addSceneObject(this->DeformableObject);

  // Same thing for the floor
  // Construct one to one nodal map based on the above meshes
  auto oneToOneFloorNodalMap = std::make_shared<imstk::OneToOneMap>();
  oneToOneFloorNodalMap->setMaster(floorMesh);
  oneToOneFloorNodalMap->setSlave(floorMesh);
  oneToOneFloorNodalMap->compute();

  auto floor = std::make_shared<imstk::PbdObject>("Floor");
  floor->setCollidingGeometry(floorMesh);
  floor->setVisualGeometry(floorMesh);
  floor->setPhysicsGeometry(floorMesh);
  floor->setPhysicsToCollidingMap(oneToOneFloorNodalMap);
  floor->setPhysicsToVisualMap(oneToOneFloorNodalMap);
  floor->setCollidingToVisualMap(oneToOneFloorNodalMap);

  auto pbdFloorModel = std::make_shared<imstk::PbdModel>();
  pbdFloorModel->setModelGeometry(floorMesh);
  pbdFloorModel->configure(/*Number of Constraints*/ 0,
    /*Mass*/ 0.0,
    /*Proximity*/ 0.1,
    /*Contact stiffness*/ 1.0);
  floor->setDynamicalModel(pbdFloorModel);

  auto pbdSolverfloor = std::make_shared<imstk::PbdSolver>();
  pbdSolverfloor->setPbdObject(floor);

  this->Scene->addNonlinearSolver(pbdSolverfloor);
  this->Scene->addSceneObject(floor);

  // Collisions
  auto colGraph = this->Scene->getCollisionGraph();
  auto pair = std::make_shared<imstk::PbdInteractionPair>(
    imstk::PbdInteractionPair(this->DeformableObject, floor));
  pair->setNumberOfInterations(2);

  colGraph->addInteractionPair(pair);

  // We need a Light :(
  auto light = std::make_shared<imstk::DirectionalLight>("Light");
  light->setFocalPoint(imstk::Vec3d(5, -8, -5));
  light->setIntensity(1);
  this->Scene->addLight(light);

  this->SDK->setActiveScene(this->Scene);
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidgetPrivate::setupOutput()
{
  Q_Q(qSlicerCollisionSimulationModuleWidget);
  // Copy input grid into the output grid
  vtkNew<vtkUnstructuredGrid> outputData;
  outputData->DeepCopy(this->InputMeshNode->GetUnstructuredGrid());
  this->OutputMeshNode->SetAndObserveMesh(outputData);

  // Also make sure the mesh is visible
  this->OutputMeshNode->CreateDefaultDisplayNodes();
  vtkMRMLDisplayNode* outputDisplayNode = this->OutputMeshNode->GetDisplayNode();
  outputDisplayNode->VisibilityOn();
}

//-----------------------------------------------------------------------------
// qSlicerCollisionSimulationModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerCollisionSimulationModuleWidget::qSlicerCollisionSimulationModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerCollisionSimulationModuleWidgetPrivate(*this) )
{
}

//-----------------------------------------------------------------------------
qSlicerCollisionSimulationModuleWidget::~qSlicerCollisionSimulationModuleWidget()
{
}

//-----------------------------------------------------------------------------
vtkSlicerCollisionSimulationLogic* qSlicerCollisionSimulationModuleWidget::simulationLogic()
{
  return vtkSlicerCollisionSimulationLogic::SafeDownCast(this->logic());
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidget::setMRMLScene(vtkMRMLScene* scene)
{
  Q_D(qSlicerCollisionSimulationModuleWidget);
  d->inputMeshNodeComboBox->setMRMLScene(scene);
  d->floorMeshNodeComboBox->setMRMLScene(scene);
  d->outputMeshNodeComboBox->setMRMLScene(scene);
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidget::setInputMeshNode(vtkMRMLNode* node)
{
  this->setInputMeshNode(vtkMRMLModelNode::SafeDownCast(node));
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidget::setInputMeshNode(vtkMRMLModelNode* node)
{
  Q_D(qSlicerCollisionSimulationModuleWidget);
  if (d->InputMeshNode == node)
    {
    return;
    }

  d->InputMeshNode = node;
  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidget::setFloorMeshNode(vtkMRMLNode* node)
{
  this->setFloorMeshNode(vtkMRMLModelNode::SafeDownCast(node));
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidget::setFloorMeshNode(vtkMRMLModelNode* node)
{
  Q_D(qSlicerCollisionSimulationModuleWidget);
  if (d->FloorMeshNode == node)
    {
    return;
    }

  d->FloorMeshNode = node;
  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidget::setOutputMeshNode(vtkMRMLNode* node)
{
  this->setOutputMeshNode(vtkMRMLModelNode::SafeDownCast(node));
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidget::setOutputMeshNode(vtkMRMLModelNode* node)
{
  Q_D(qSlicerCollisionSimulationModuleWidget);
  if (d->OutputMeshNode == node)
  {
    return;
  }

  d->OutputMeshNode = node;
  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidget::setup()
{
  Q_D(qSlicerCollisionSimulationModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  // Icons setup
  d->stopPushButton->setIcon(
    d->stopPushButton->style()->standardIcon(QStyle::SP_MediaStop));
  d->playPushButton->setIcon(
    d->playPushButton->style()->standardIcon(QStyle::SP_MediaPlay));
  d->pausePushButton->setIcon(
    d->pausePushButton->style()->standardIcon(QStyle::SP_MediaPause));

  QSize warningIconSize(16, d->inputMeshNodeComboBox->height());
  d->inputMeshWarningLabel->setPixmap(
    d->inputMeshWarningLabel->style()->standardIcon(QStyle::SP_MessageBoxWarning).pixmap(warningIconSize));

  // Signals connections
  d->inputMeshNodeComboBox->connect(
    d->inputMeshNodeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
    this, SLOT(setInputMeshNode(vtkMRMLNode*)));
  d->floorMeshNodeComboBox->connect(
    d->floorMeshNodeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
    this, SLOT(setFloorMeshNode(vtkMRMLNode*)));
  d->outputMeshNodeComboBox->connect(
    d->outputMeshNodeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
    this, SLOT(setOutputMeshNode(vtkMRMLNode*)));

  d->stopPushButton->connect(
    d->stopPushButton, SIGNAL(clicked()),
    this, SLOT(endSimulation()));
  d->playPushButton->connect(
    d->playPushButton, SIGNAL(clicked()),
    this, SLOT(startSimulation()));
  d->pausePushButton->connect(
    d->pausePushButton, SIGNAL(clicked()),
    this, SLOT(pauseSimulation()));

  // Create the SDK
  d->SDK = std::make_shared<imstk::SimulationManager>(false, false);

  // Timer setup
  d->MeshUpdateTimer.setInterval(7);
  d->MeshUpdateTimer.connect(
    &(d->MeshUpdateTimer), SIGNAL(timeout()),
    this, SLOT(updateFromSimulation()));

  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidget::updateWidgetFromMRML()
{
  Q_D(qSlicerCollisionSimulationModuleWidget);
  bool freezeInputs =
    this->simulationStatus() == static_cast<int>(imstk::SimulationStatus::RUNNING);

  // input mesh
  d->inputMeshNodeComboBox->setEnabled(!freezeInputs);
  bool isInputVolumetricMesh =
    d->InputMeshNode && d->InputMeshNode->GetUnstructuredGrid();
  d->inputMeshWarningLabel->setVisible(
    d->InputMeshNode && !d->InputMeshNode->GetUnstructuredGrid());

  // floor mesh
  d->floorMeshNodeComboBox->setEnabled(!freezeInputs);
  bool isFloorVolumetricMesh =
    d->FloorMeshNode && d->FloorMeshNode->GetUnstructuredGrid();

  // output mesh
  d->outputMeshNodeComboBox->setEnabled(!freezeInputs);
  bool isOutputSet = d->OutputMeshNode != nullptr;

  // Simulation controls
  bool areInputsValid =
    isInputVolumetricMesh &&
    isFloorVolumetricMesh &&
    isOutputSet;
  d->stopPushButton->setEnabled(areInputsValid);
  d->playPushButton->setEnabled(areInputsValid);
  d->pausePushButton->setEnabled(areInputsValid);
}

//-----------------------------------------------------------------------------
int qSlicerCollisionSimulationModuleWidget::simulationStatus()
{
  Q_D(qSlicerCollisionSimulationModuleWidget);
  return static_cast<int>(d->SDK->getStatus());
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidget::startSimulation()
{
  Q_D(qSlicerCollisionSimulationModuleWidget);

  d->setupSimulation();
  d->setupOutput();
  d->MeshUpdateTimer.start();
  d->SDK->startSimulation(imstk::SimulationStatus::RUNNING);
  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidget::pauseSimulation()
{
  Q_D(qSlicerCollisionSimulationModuleWidget);
  d->SDK->pauseSimulation();
  d->MeshUpdateTimer.stop();
  this->updateFromSimulation();
  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidget::endSimulation()
{
  Q_D(qSlicerCollisionSimulationModuleWidget);
  d->SDK->endSimulation();
  d->MeshUpdateTimer.stop();
  this->updateFromSimulation();
  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidget::updateFromSimulation()
{
  Q_D(qSlicerCollisionSimulationModuleWidget);
  Q_ASSERT(d->OutputMeshNode && d->OutputMeshNode->GetUnstructuredGrid());

  imstk::StdVectorOfVec3d newPoints =
    d->DeformableModel->getCurrentState()->getPositions();

  int wasModifying = d->OutputMeshNode->StartModify();
  vtkUnstructuredGrid* data = d->OutputMeshNode->GetUnstructuredGrid();
  vtkPoints* dataPoints = data->GetPoints();

  for (int i = 0; i < dataPoints->GetNumberOfPoints(); i++)
    {
    dataPoints->SetPoint(i, newPoints[i][0], newPoints[i][1], newPoints[i][2]);
    }
  data->SetPoints(dataPoints);
  data->Modified();

  d->OutputMeshNode->SetAndObserveMesh(data);
  d->OutputMeshNode->EndModify(wasModifying);
}
