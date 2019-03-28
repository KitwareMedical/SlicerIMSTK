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
#include "imstkSimulationManager.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerCollisionSimulationModuleWidgetPrivate: public Ui_qSlicerCollisionSimulationModuleWidget
{
public:
  qSlicerCollisionSimulationModuleWidgetPrivate();

  vtkMRMLModelNode* ModelNode;

  std::shared_ptr<imstk::SimulationManager> SDK;
  bool IsInitialized;
};

//-----------------------------------------------------------------------------
// qSlicerCollisionSimulationModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerCollisionSimulationModuleWidgetPrivate::qSlicerCollisionSimulationModuleWidgetPrivate()
{
  this->ModelNode = nullptr;
  this->IsInitialized = false;
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
  d->volumetricMeshNodeComboBox->setMRMLScene(scene);
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidget::setVolumeNode(vtkMRMLNode* node)
{
  this->setVolumeNode(vtkMRMLModelNode::SafeDownCast(node));
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidget::setVolumeNode(vtkMRMLModelNode* node)
{
  Q_D(qSlicerCollisionSimulationModuleWidget);
  if (d->ModelNode == node)
    {
    return;
    }

  d->ModelNode = node;
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
  d->volumetricMeshNodeComboBox->connect(
    d->volumetricMeshNodeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
    this, SLOT(setVolumeNode(vtkMRMLNode*)));

  d->stopPushButton->connect(
    d->stopPushButton, SIGNAL(clicked()),
    this, SLOT(endSimulation()));
  d->playPushButton->connect(
    d->playPushButton, SIGNAL(clicked()),
    this, SLOT(startSimulation()));
  d->pausePushButton->connect(
    d->pausePushButton, SIGNAL(clicked()),
    this, SLOT(pauseSimulation()));
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
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidget::pauseSimulation()
{
  Q_D(qSlicerCollisionSimulationModuleWidget);
  // \todo
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModuleWidget::endSimulation()
{
  Q_D(qSlicerCollisionSimulationModuleWidget);
  // \todo
}
