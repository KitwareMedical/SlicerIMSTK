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

// MRML Includes
#include "vtkMRMLNode.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLModelDisplayNode.h"
#include "vtkMRMLTransformDisplayNode.h"

// Slicer includes
#include "qSlicerIMSTKModuleWidget.h"
#include "ui_qSlicerIMSTKModuleWidget.h"
#include "vtkSlicerIMSTKLogic.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerIMSTKModuleWidgetPrivate: public Ui_qSlicerIMSTKModuleWidget
{
  Q_DECLARE_PUBLIC(qSlicerIMSTKModuleWidget)
public:
  qSlicerIMSTKModuleWidgetPrivate(qSlicerIMSTKModuleWidget& object);
  vtkSlicerIMSTKLogic* logic() const;
  int hapticSimulationTag;
  int rigidSimulationTag;

private:
  qSlicerIMSTKModuleWidget* const q_ptr;
};

//-----------------------------------------------------------------------------
// qSlicerIMSTKModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerIMSTKModuleWidgetPrivate::qSlicerIMSTKModuleWidgetPrivate(qSlicerIMSTKModuleWidget& object)
  : q_ptr(&object),
  hapticSimulationTag(-1),
  rigidSimulationTag(-1)
{
}

vtkSlicerIMSTKLogic* qSlicerIMSTKModuleWidgetPrivate::logic() const
{
  Q_Q(const qSlicerIMSTKModuleWidget);
  return vtkSlicerIMSTKLogic::SafeDownCast(q->logic());
}

//-----------------------------------------------------------------------------
// qSlicerIMSTKModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerIMSTKModuleWidget::qSlicerIMSTKModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerIMSTKModuleWidgetPrivate(*this ))
{
}

//-----------------------------------------------------------------------------
qSlicerIMSTKModuleWidget::~qSlicerIMSTKModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerIMSTKModuleWidget::setup()
{
  Q_D(qSlicerIMSTKModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();
  

  this->connect(d->RigidBodyApplyButton, SIGNAL(clicked()), this, SLOT(onRigidBodyApplyButton()));
  this->connect(d->HapticApplyButton, SIGNAL(clicked()), this, SLOT(onHapticApplyButton()));
  this->connect(d->RigidStopButton, SIGNAL(clicked()), this, SLOT(onRigidStopButton()));
  this->connect(d->HapticStopButton, SIGNAL(clicked()), this, SLOT(onHapticStopButton()));
  this->connect(d->RigidBodyInputModelComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),this, SLOT(onRigidBodyInputsChanged(vtkMRMLNode*)));
  this->connect(d->RigidBodyOutputModelComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT(onRigidBodyInputsChanged(vtkMRMLNode*)));
  this->connect(d->RigidBodyOutputTransformComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT(onRigidBodyInputsChanged(vtkMRMLNode*)));
  this->connect(d->HapticOutputTransformComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT(onHapticInputsChanged(vtkMRMLNode*)));

  d->RigidBodyApplyButton->setEnabled(false);
  d->HapticApplyButton->setEnabled(false);
  d->HapticStopButton->setEnabled(false);
  d->RigidStopButton->setEnabled(false);
}

void qSlicerIMSTKModuleWidget::onRigidBodyApplyButton()
{
  Q_D(qSlicerIMSTKModuleWidget);

  std::cout << "Clicked rigid body apply" << std::endl;
  d->RigidStopButton->setEnabled(true);
  d->RigidBodyApplyButton->setEnabled(false);

  auto input = vtkMRMLModelNode::SafeDownCast(d->RigidBodyInputModelComboBox->currentNode());
  auto output = vtkMRMLModelNode::SafeDownCast(d->RigidBodyOutputModelComboBox->currentNode());
  auto transform = vtkMRMLLinearTransformNode::SafeDownCast(d->RigidBodyOutputTransformComboBox->currentNode());
  input->CreateDefaultDisplayNodes();
  output->CreateDefaultDisplayNodes();
  input->GetModelDisplayNode()->SetVisibility(false);
  output->GetModelDisplayNode()->SetVisibility(true);

  d->logic()->runObjectCtrlDummyClientExample("RigidBody", input, output, transform);
}

void qSlicerIMSTKModuleWidget::onHapticApplyButton()
{
  Q_D(qSlicerIMSTKModuleWidget);

  std::cout << "Clicked haptics apply" << std::endl;
  d->HapticStopButton->setEnabled(true);
  d->HapticApplyButton->setEnabled(false);

  auto transform = vtkMRMLLinearTransformNode::SafeDownCast(d->HapticOutputTransformComboBox->currentNode());
  transform->CreateDefaultDisplayNodes();
  auto disp = vtkMRMLTransformDisplayNode::SafeDownCast(transform->GetDisplayNode());
  disp->SetEditorVisibility(true);

  d->logic()->runHapticDeviceExample("HapticDevice",d->HapticDeviceNameLineEdit->text().toStdString(), transform);


}

void qSlicerIMSTKModuleWidget::onRigidStopButton()
{
  Q_D(qSlicerIMSTKModuleWidget);
  d->logic()->stopSimulation("RigidBody");
  d->RigidStopButton->setEnabled(false);

  this->onRigidBodyInputsChanged(nullptr);
}

void qSlicerIMSTKModuleWidget::onHapticStopButton()
{
  Q_D(qSlicerIMSTKModuleWidget);
  d->logic()->stopSimulation("HapticDevice");
  d->HapticStopButton->setEnabled(false);
  this->onHapticInputsChanged(nullptr);
}

void qSlicerIMSTKModuleWidget::onRigidBodyInputsChanged(vtkMRMLNode* unused)
{
  Q_D(qSlicerIMSTKModuleWidget);

  std::cout << "changed rigid body inputs" << std::endl;
  d->RigidBodyApplyButton->setEnabled(d->RigidBodyInputModelComboBox->currentNode() != nullptr &&
                                      d->RigidBodyOutputModelComboBox->currentNode() != nullptr &&
                                      d->RigidBodyOutputTransformComboBox->currentNode() != nullptr&&
                                      d->RigidBodyInputModelComboBox->currentNodeID() != d->RigidBodyOutputModelComboBox->currentNodeID());
}

void qSlicerIMSTKModuleWidget::onHapticInputsChanged(vtkMRMLNode* unused)
{
  Q_D(qSlicerIMSTKModuleWidget);

  std::cout << "changed haptic inputs" << std::endl;

  d->HapticApplyButton->setEnabled(d->HapticOutputTransformComboBox->currentNode() != nullptr);  
}
