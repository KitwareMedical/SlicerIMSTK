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

// SlicerQt includes
#include "vtkSlicerCollisionSimulationLogic.h"
#include "qSlicerCollisionSimulationModuleWidget.h"
#include "ui_qSlicerCollisionSimulationModuleWidget.h"

// Slicer includes
#include <vtkMRMLModelNode.h>

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
public:
  qSlicerCollisionSimulationModuleWidgetPrivate();

  vtkMRMLModelNode* InputMeshNode;
  vtkMRMLModelNode* FloorMeshNode;

  std::shared_ptr<imstk::Scene> Scene;
  std::shared_ptr<imstk::SimulationManager> SDK;

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
qSlicerCollisionSimulationModuleWidgetPrivate::qSlicerCollisionSimulationModuleWidgetPrivate()
{
  this->InputMeshNode = nullptr;
}

//-----------------------------------------------------------------------------
// qSlicerCollisionSimulationModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerCollisionSimulationModuleWidget::qSlicerCollisionSimulationModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerCollisionSimulationModuleWidgetPrivate )
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

  // Signals connections
  d->inputMeshNodeComboBox->connect(
    d->inputMeshNodeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
    this, SLOT(setInputMeshNode(vtkMRMLNode*)));
  d->floorMeshNodeComboBox->connect(
    d->floorMeshNodeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
    this, SLOT(setFloorMeshNode(vtkMRMLNode*)));

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
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidget::updateWidgetFromMRML()
{
  Q_D(qSlicerCollisionSimulationModuleWidget);

  // Updates \todo
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidget::startSimulation()
{
  Q_D(qSlicerCollisionSimulationModuleWidget);

  // Do you need to set the camera

  // Do you need to have two models ? One to follow and one to deform ?

  // Create a new scene
  d->Scene = d->SDK->createNewScene();

  // Get the mesh from its filename
  const char* meshFilename = this->simulationLogic()->ForceGetNodeFileName(d->InputMeshNode);
  auto mesh = imstk::MeshIO::read(meshFilename);

  const char* floorMeshFilename =
    this->simulationLogic()->ForceGetNodeFileName(d->FloorMeshNode);
  auto floorMesh = imstk::MeshIO::read(floorMeshFilename);

  // Extract the surface mesh from the tetrahedral mesh
  auto volTetMesh = std::dynamic_pointer_cast<imstk::TetrahedralMesh>(mesh);
  volTetMesh->scale(d->geoScalingFactor, imstk::Geometry::TransformType::ApplyToData);
  auto surfMesh = std::make_shared<imstk::SurfaceMesh>();
  volTetMesh->extractSurfaceMesh(surfMesh, true);

  // Construct one to one nodal map based on the above meshes
  auto oneToOneNodalMap = std::make_shared<imstk::OneToOneMap>();
  oneToOneNodalMap->setMaster(volTetMesh);
  oneToOneNodalMap->setSlave(surfMesh);
  oneToOneNodalMap->compute();

  // Scene Object
  auto deformableObj = std::make_shared<imstk::PbdObject>("DeformableObject");
  deformableObj->setCollidingGeometry(surfMesh);
  deformableObj->setVisualGeometry(surfMesh);
  deformableObj->setPhysicsGeometry(volTetMesh);
  deformableObj->setPhysicsToCollidingMap(oneToOneNodalMap);
  deformableObj->setPhysicsToVisualMap(oneToOneNodalMap);
  deformableObj->setCollidingToVisualMap(oneToOneNodalMap);

  // Create model
  auto pdbModel = std::make_shared<imstk::PbdModel>();
  pdbModel->setModelGeometry(volTetMesh);
  pdbModel->configure( //\todo Make this configurable
    /*Number of Constraints*/ 1,
    /*Constraint configuration*/ "FEM NeoHookean 1.0 0.3",
    /*Mass*/ 1.0,
    /*Gravity*/ "0 -9.8 0",
    /*TimeStep*/ 0.001,
    /*FixedPoint*/ "",
    /*NumberOfIterationInConstraintSolver*/ 2,
    /*Proximity*/ 0.1,
    /*Contact stiffness*/ 0.01
  );
  deformableObj->setDynamicalModel(pdbModel);

  // Create solver
  auto pbdSolver = std::make_shared<imstk::PbdSolver>();
  pbdSolver->setPbdObject(deformableObj);

  d->Scene->addNonlinearSolver(pbdSolver);
  d->Scene->addSceneObject(deformableObj);

  // Same thing for the floor
  auto floorVolTetMesh = std::dynamic_pointer_cast<imstk::TetrahedralMesh>(floorMesh);
  floorVolTetMesh->scale(d->geoScalingFactor, imstk::Geometry::TransformType::ApplyToData);
  auto floorSurfMesh = std::make_shared<imstk::SurfaceMesh>();
  floorVolTetMesh->extractSurfaceMesh(floorSurfMesh, true);

  // Construct one to one nodal map based on the above meshes
  auto oneToOneFloorNodalMap = std::make_shared<imstk::OneToOneMap>();
  oneToOneFloorNodalMap->setMaster(floorVolTetMesh);
  oneToOneFloorNodalMap->setSlave(floorSurfMesh);
  oneToOneFloorNodalMap->compute();

  auto floor = std::make_shared<imstk::PbdObject>("Floor");
  floor->setCollidingGeometry(floorMesh);
  floor->setVisualGeometry(floorMesh);
  floor->setPhysicsGeometry(floorMesh);
  floor->setPhysicsToCollidingMap(oneToOneFloorNodalMap);
  floor->setPhysicsToVisualMap(oneToOneFloorNodalMap);
  floor->setCollidingToVisualMap(oneToOneFloorNodalMap);

  auto pbdSolverfloor = std::make_shared<imstk::PbdSolver>();
  pbdSolverfloor->setPbdObject(floor);

  d->Scene->addNonlinearSolver(pbdSolverfloor);
  d->Scene->addSceneObject(floor);

  // Collisions
  auto colGraph = d->Scene->getCollisionGraph();
  auto pair = std::make_shared<imstk::PbdInteractionPair>(
    imstk::PbdInteractionPair(deformableObj, floor));
  pair->setNumberOfInterations(2);

  colGraph->addInteractionPair(pair);

  // We need a Light :(
  auto light = std::make_shared<imstk::DirectionalLight>("Light");
  light->setFocalPoint(imstk::Vec3d(5, -8, -5));
  light->setIntensity(1);
  d->Scene->addLight(light);

  d->SDK->setActiveScene(d->Scene);
  d->SDK->startSimulation(imstk::SimulationStatus::RUNNING);
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidget::pauseSimulation()
{
  Q_D(qSlicerCollisionSimulationModuleWidget);
  d->SDK->pauseSimulation();
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidget::endSimulation()
{
  Q_D(qSlicerCollisionSimulationModuleWidget);
  // \todo
}
