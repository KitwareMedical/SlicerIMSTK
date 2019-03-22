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

// Floop Logic includes
#include <vtkSlicerFloopLogic.h>

// Floop includes
#include "qSlicerFloopModule.h"
#include "qSlicerFloopModuleWidget.h"

//-----------------------------------------------------------------------------
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include <QtPlugin>
Q_EXPORT_PLUGIN2(qSlicerFloopModule, qSlicerFloopModule);
#endif

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerFloopModulePrivate
{
public:
  qSlicerFloopModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerFloopModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerFloopModulePrivate::qSlicerFloopModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerFloopModule methods

//-----------------------------------------------------------------------------
qSlicerFloopModule::qSlicerFloopModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerFloopModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerFloopModule::~qSlicerFloopModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerFloopModule::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerFloopModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerFloopModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerFloopModule::icon() const
{
  return QIcon(":/Icons/Floop.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerFloopModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerFloopModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerFloopModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerFloopModule
::createWidgetRepresentation()
{
  return new qSlicerFloopModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerFloopModule::createLogic()
{
  return vtkSlicerFloopLogic::New();
}
