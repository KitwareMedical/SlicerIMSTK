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

#ifndef __qSlicerIMSTKModuleWidget_h
#define __qSlicerIMSTKModuleWidget_h

// Slicer includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerIMSTKModuleExport.h"

class qSlicerIMSTKModuleWidgetPrivate;
class vtkMRMLNode;


class Q_SLICER_QTMODULES_IMSTK_EXPORT qSlicerIMSTKModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerIMSTKModuleWidget(QWidget *parent=0);
  virtual ~qSlicerIMSTKModuleWidget();

public slots:

  void onRigidBodyApplyButton();
  void onHapticApplyButton();
  void onRigidStopButton();
  void onHapticStopButton();
  void onRigidBodyInputsChanged(vtkMRMLNode* node);
  void onHapticInputsChanged(vtkMRMLNode* node);

protected:
  QScopedPointer<qSlicerIMSTKModuleWidgetPrivate> d_ptr;

  void setup() override;

private:
  Q_DECLARE_PRIVATE(qSlicerIMSTKModuleWidget);
  Q_DISABLE_COPY(qSlicerIMSTKModuleWidget);
};

#endif
