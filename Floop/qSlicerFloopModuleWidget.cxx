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
#include "qSlicerFloopModuleWidget.h"
#include "ui_qSlicerFloopModuleWidget.h"

// Slicer includes
#include <vtkMRMLModelNode.h>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerFloopModuleWidgetPrivate: public Ui_qSlicerFloopModuleWidget
{
public:
  qSlicerFloopModuleWidgetPrivate();

  vtkMRMLModelNode* ModelNode;
};

//-----------------------------------------------------------------------------
// qSlicerFloopModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerFloopModuleWidgetPrivate::qSlicerFloopModuleWidgetPrivate()
{
  this->ModelNode = nullptr;
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
}

//-----------------------------------------------------------------------------
void qSlicerFloopModuleWidget::updateWidgetFromMRML()
{
  Q_D(qSlicerFloopModuleWidget);

  // Updates \todo
}
