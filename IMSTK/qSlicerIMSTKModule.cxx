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

// IMSTK Logic includes
#include <vtkSlicerIMSTKLogic.h>

// IMSTK includes
#include "qSlicerIMSTKModule.h"
#include "qSlicerIMSTKModuleWidget.h"

//-----------------------------------------------------------------------------
class qSlicerIMSTKModulePrivate
{
public:
  qSlicerIMSTKModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerIMSTKModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerIMSTKModulePrivate::qSlicerIMSTKModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerIMSTKModule methods

//-----------------------------------------------------------------------------
qSlicerIMSTKModule::qSlicerIMSTKModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerIMSTKModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerIMSTKModule::~qSlicerIMSTKModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerIMSTKModule::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerIMSTKModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerIMSTKModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerIMSTKModule::icon() const
{
  return QIcon(":/Icons/IMSTK.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerIMSTKModule::categories() const
{
  return QStringList() << "IMSTK";
}

//-----------------------------------------------------------------------------
QStringList qSlicerIMSTKModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerIMSTKModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerIMSTKModule
::createWidgetRepresentation()
{
  return new qSlicerIMSTKModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerIMSTKModule::createLogic()
{
  return vtkSlicerIMSTKLogic::New();
}
