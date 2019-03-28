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

// CollisionSimulation Logic includes
#include <vtkSlicerCollisionSimulationLogic.h>

// CollisionSimulation includes
#include "qSlicerCollisionSimulationModule.h"
#include "qSlicerCollisionSimulationModuleWidget.h"

//-----------------------------------------------------------------------------
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include <QtPlugin>
Q_EXPORT_PLUGIN2(qSlicerCollisionSimulationModule, qSlicerCollisionSimulationModule);
#endif

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerCollisionSimulationModulePrivate
{
public:
  qSlicerCollisionSimulationModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerCollisionSimulationModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerCollisionSimulationModulePrivate::qSlicerCollisionSimulationModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerCollisionSimulationModule methods

//-----------------------------------------------------------------------------
qSlicerCollisionSimulationModule::qSlicerCollisionSimulationModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerCollisionSimulationModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerCollisionSimulationModule::~qSlicerCollisionSimulationModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerCollisionSimulationModule::helpText() const
{
  return "Tihs is a simple example of simulation intergation between iMSTK and Slicer.";
}

//-----------------------------------------------------------------------------
QString qSlicerCollisionSimulationModule::acknowledgementText() const
{
  return "";
}

//-----------------------------------------------------------------------------
QStringList qSlicerCollisionSimulationModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("Johan Andruejol (Kitware Inc.");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerCollisionSimulationModule::icon() const
{
  return QIcon(":/Icons/CollisionSimulation.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerCollisionSimulationModule::categories() const
{
  return QStringList() << "Simulation";
}

//-----------------------------------------------------------------------------
QStringList qSlicerCollisionSimulationModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerCollisionSimulationModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerCollisionSimulationModule
::createWidgetRepresentation()
{
  return new qSlicerCollisionSimulationModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerCollisionSimulationModule::createLogic()
{
  return vtkSlicerCollisionSimulationLogic::New();
}
