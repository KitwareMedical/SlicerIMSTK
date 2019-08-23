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
#include <vtkMRMLCommandLineModuleNode.h>
#include <vtkMRMLDisplayNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLLinearTransformNode.h>
#include <vtkMRMLTransformDisplayNode.h>
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkUnstructuredGrid.h>

// iMSTK includes
#include <imstkSimulationManager.h>  //We want to get rid of even this eventually

// Qt includes
#include <QDebug>

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
  void setupFloorTransform();

  std::shared_ptr<imstk::SimulationManager> SDK();

  QTimer MeshUpdateTimer;

  vtkMRMLModelNode* InputMeshNode;
  vtkMRMLModelNode* FloorMeshNode;
  vtkMRMLModelNode* OutputMeshNode;
  vtkSmartPointer<vtkMRMLLinearTransformNode> FloorTransformNode;
  vtkMRMLScene* scene;

  std::string InputMeshNodeName;
  int index;  
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
  this->FloorTransformNode = nullptr;
  this->scene = nullptr;
  this->index = -1;
}

//-----------------------------------------------------------------------------
std::shared_ptr<imstk::SimulationManager>
qSlicerCollisionSimulationModuleWidgetPrivate::SDK()
{
  Q_Q(qSlicerCollisionSimulationModuleWidget);
  return q->simulationLogic()->GetSDK();
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidgetPrivate::setupFloorTransform()
{
  if (this->FloorTransformNode)
  {
    this->scene->RemoveNode(this->FloorTransformNode);
    this->FloorTransformNode = nullptr;
  }

  this->FloorTransformNode = vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
  this->FloorTransformNode->SetName(this->FloorMeshNode->GetName());
  this->scene->AddNode(this->FloorTransformNode);
  this->FloorTransformNode->CreateDefaultDisplayNodes();
  this->FloorMeshNode->SetAndObserveTransformNodeID(this->FloorTransformNode->GetID());
  auto disp = this->FloorTransformNode->GetDisplayNode();
  auto transform_disp = vtkMRMLTransformDisplayNode::SafeDownCast(disp);
  transform_disp->SetEditorScalingEnabled(false);
  transform_disp->SetEditorVisibility(true);
  transform_disp->UpdateEditorBounds();
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidgetPrivate::setupSimulation()
{
  Q_Q(qSlicerCollisionSimulationModuleWidget);
  // Do you need to have two models ? One to follow and one to deform ?
  this->index++;
  // Create a new scene
  std::string sceneName = "CollisionSimulation" + std::to_string(this->index);
  q->simulationLogic()->CreateScene(sceneName, false);
  auto scene = this->SDK()->getScene(sceneName);

  // Build mesh geometry
  q->simulationLogic()->AddDeformableObject(
    sceneName,
    this->InputMeshNode, this->Gravity->value(), this->Stiffness->value(), this->Dt->value(),
    this->Youngs->value(), this->Poisson->value(), this->Mass->value());
  this->InputMeshNodeName = this->InputMeshNode->GetName();

  // Build floor geometry
  q->simulationLogic()->AddImmovableObject(
    sceneName,
    this->FloorMeshNode,
    this->Dt->value());

  //Attach controller to floor geometry;
  //q->simulationLogic()->AttachTransformController(sceneName, this->FloorMeshNode->GetName(), this->FloorTransformNode);

  // Collisions
  q->simulationLogic()->AddCollisionInteraction(sceneName,
    this->InputMeshNode->GetName(), this->FloorMeshNode->GetName());

  // Set the scene
  q->simulationLogic()->SetActiveScene(sceneName, true);
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
  d->scene = scene;
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
  this->simulationLogic()->CreateSDK(
    /*disableRendering=*/false,
    /*enableVR=*/false
    );

  // Timer setup
  d->MeshUpdateTimer.setInterval(10);
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
  return static_cast<int>(d->SDK()->getStatus());
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidget::startSimulation()
{
  Q_D(qSlicerCollisionSimulationModuleWidget);
  
  //d->setupFloorTransform();
  d->setupSimulation();
  d->setupOutput();
  d->MeshUpdateTimer.start();
  vtkMRMLDisplayNode* outputDisplayNode = d->InputMeshNode->GetDisplayNode();
  outputDisplayNode->VisibilityOff();
  d->SDK()->startSimulation(imstk::SimulationStatus::RUNNING);
  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidget::pauseSimulation()
{
  Q_D(qSlicerCollisionSimulationModuleWidget);
  if (d->SDK()->getStatus() != imstk::SimulationStatus::RUNNING)
    {
    return;
    }

  d->SDK()->pauseSimulation();
  d->MeshUpdateTimer.stop();
  this->updateFromSimulation();
  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidget::endSimulation()
{
  Q_D(qSlicerCollisionSimulationModuleWidget);
  if (d->SDK()->getStatus() != imstk::SimulationStatus::RUNNING
    || d->SDK()->getStatus() != imstk::SimulationStatus::PAUSED)
    {
    return;
    }

  d->SDK()->endSimulation();
  d->MeshUpdateTimer.stop();
  this->updateFromSimulation();
  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidget::updateFromSimulation()
{
  Q_D(qSlicerCollisionSimulationModuleWidget);
  Q_ASSERT(d->OutputMeshNode && d->OutputMeshNode->GetUnstructuredGrid());
  this->simulationLogic()->UpdateMeshPointsFromObject(
    d->InputMeshNodeName, d->OutputMeshNode);
  this->simulationLogic()->UpdateAssociatedController(d->FloorTransformNode);
}

