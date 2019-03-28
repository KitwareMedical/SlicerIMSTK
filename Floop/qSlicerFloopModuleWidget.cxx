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
#include "vtkSlicerFloopLogic.h"
#include "qSlicerFloopModuleWidget.h"
#include "ui_qSlicerFloopModuleWidget.h"

// Slicer includes
#include <vtkMRMLModelNode.h>

// iMSTK includes
#include "imstkSimulationManager.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerFloopModuleWidgetPrivate: public Ui_qSlicerFloopModuleWidget
{
public:
  qSlicerFloopModuleWidgetPrivate();

  vtkMRMLModelNode* ModelNode;

  std::shared_ptr<imstk::SimulationManager> SDK;
  bool IsInitialized;
};

//-----------------------------------------------------------------------------
// qSlicerFloopModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerFloopModuleWidgetPrivate::qSlicerFloopModuleWidgetPrivate()
{
  this->ModelNode = nullptr;
  this->IsInitialized = false;
}

//-----------------------------------------------------------------------------
// qSlicerFloopModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerFloopModuleWidget::qSlicerFloopModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerFloopModuleWidgetPrivate )
{
}

//-----------------------------------------------------------------------------
qSlicerFloopModuleWidget::~qSlicerFloopModuleWidget()
{
}

//-----------------------------------------------------------------------------
vtkSlicerFloopLogic* qSlicerFloopModuleWidget::simulationLogic()
{
  return vtkSlicerFloopLogic::SafeDownCast(this->logic());
}

//-----------------------------------------------------------------------------
void qSlicerFloopModuleWidget::setMRMLScene(vtkMRMLScene* scene)
{
  Q_D(qSlicerFloopModuleWidget);
  d->volumetricMeshNodeComboBox->setMRMLScene(scene);
}

//-----------------------------------------------------------------------------
void qSlicerFloopModuleWidget::setVolumeNode(vtkMRMLNode* node)
{
  this->setVolumeNode(vtkMRMLModelNode::SafeDownCast(node));
}

//-----------------------------------------------------------------------------
void qSlicerFloopModuleWidget::setVolumeNode(vtkMRMLModelNode* node)
{
  Q_D(qSlicerFloopModuleWidget);
  if (d->ModelNode == node)
    {
    return;
    }

  d->ModelNode = node;
  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSlicerFloopModuleWidget::setup()
{
  Q_D(qSlicerFloopModuleWidget);
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
void qSlicerFloopModuleWidget::updateWidgetFromMRML()
{
  Q_D(qSlicerFloopModuleWidget);

  // Updates \todo
}

//-----------------------------------------------------------------------------
void qSlicerFloopModuleWidget::startSimulation()
{
  Q_D(qSlicerFloopModuleWidget);
}

//-----------------------------------------------------------------------------
void qSlicerFloopModuleWidget::pauseSimulation()
{
  Q_D(qSlicerFloopModuleWidget);
  // \todo
}

//-----------------------------------------------------------------------------
void qSlicerFloopModuleWidget::endSimulation()
{
  Q_D(qSlicerFloopModuleWidget);
  // \todo
}
