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
#include "vtkSlicerIMSTKLogic.h"

// MRML includes
#include <vtkMRMLLinearTransformNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLScene.h>

// iMSTK includes
#include "imstkCamera.h"
#include "imstkCollidingObject.h"
#include "imstkDirectionalLight.h"
#include "imstkDummyClient.h"
#include "imstkGeometryUtilities.h"
#include "imstkHapticDeviceClient.h"
#include "imstkHapticDeviceManager.h"
#include "imstkKeyboardSceneControl.h"
#include "imstkMouseSceneControl.h"
#include "imstkNew.h"
#include "imstkScene.h"
#include "imstkSceneManager.h"
#include "imstkSceneObjectController.h"
#include "imstkSceneObject.h"
#include "imstkSimulationManager.h"
#include "imstkSurfaceMesh.h"
#include "imstkVisualModel.h"
#include "imstkVTKViewer.h"

// VTK includes
#include <vtkIntArray.h>
#include <vtkMatrix4x4.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>

// STD includes
#include <cassert>


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerIMSTKLogic);

//----------------------------------------------------------------------------
vtkSlicerIMSTKLogic::vtkSlicerIMSTKLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerIMSTKLogic::~vtkSlicerIMSTKLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerIMSTKLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkSlicerIMSTKLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerIMSTKLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerIMSTKLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerIMSTKLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerIMSTKLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}

void vtkSlicerIMSTKLogic::runHapticDeviceExample(std::string simName, std::string deviceName, vtkMRMLLinearTransformNode* outputTransformNode)
{
  imstk::imstkNew<imstk::Scene>               scene("SDFHaptics");

  {
    scene->getActiveCamera()->setPosition(-2.3, 23.81, 45.65);
    scene->getActiveCamera()->setFocalPoint(9.41, 8.45, 5.76);

    // Light (white)
    imstk::imstkNew<imstk::DirectionalLight> whiteLight;
    {
      whiteLight->setDirection(imstk::Vec3d(0.0, -8.0, -5.0));
      whiteLight->setIntensity(1.0);
      scene->addLight("whitelight", whiteLight);
    }
  }

  imstk::imstkNew<imstk::HapticDeviceManager>       hapticManager;
  std::shared_ptr<imstk::HapticDeviceClient> client = hapticManager->makeDeviceClient(deviceName);

  // Run the simulation
  {
    // Setup a viewer to render in its own thread
    imstk::imstkNew<imstk::VTKViewer> viewer;
    viewer->setActiveScene(scene);

    // Setup a scene manager to advance the scene in its own thread
    imstk::imstkNew<imstk::SceneManager> sceneManager;
    sceneManager->setActiveScene(scene);

    imstk::imstkNew<imstk::SimulationManager> driver;
    driver->addModule(viewer);
    driver->addModule(sceneManager);
    driver->addModule(hapticManager);

    imstk::connect<imstk::Event>(sceneManager, &imstk::SceneManager::postUpdate, [&](imstk::Event*)
      {
        auto orientation = client->getOrientation();
        auto position = client->getPosition();
        vtkNew<vtkMatrix4x4> matrix;

        Eigen::Affine3d t(Eigen::Translation3d(position[0], position[1], position[2]));
        Eigen::Affine3d r(orientation.normalized().toRotationMatrix());
        Eigen::Matrix4d m = t.matrix();
        m *= r.matrix();
        //Eigen is default column major
        //VTK is default row major
        matrix->DeepCopy(m.data());
        matrix->Transpose();

        outputTransformNode->SetMatrixTransformToParent(matrix.GetPointer());
      });

    // Add mouse and keyboard controls to the viewer
    {
      imstk::imstkNew<imstk::MouseSceneControl> mouseControl(viewer->getMouseDevice());
      mouseControl->setSceneManager(sceneManager);
      viewer->addControl(mouseControl);

      imstk::imstkNew<imstk::KeyboardSceneControl> keyControl(viewer->getKeyboardDevice());
      keyControl->setSceneManager(sceneManager);
      keyControl->setModuleDriver(driver);
      viewer->addControl(keyControl);
    }

    this->simulations[simName] = driver;
    driver->start();
  }

}

void vtkSlicerIMSTKLogic::runObjectCtrlDummyClientExample(std::string simName, vtkMRMLModelNode* inputNode, vtkMRMLModelNode* outputNode, vtkMRMLLinearTransformNode* outputTransformNode)
{

  imstk::imstkNew<imstk::Scene> scene("ObjectControllerDummyClient");

  auto geom = imstk::GeometryUtils::copyToSurfaceMesh(inputNode->GetPolyData());

  imstk::imstkNew<imstk::CollidingObject> object("VirtualObject");
  object->setVisualGeometry(geom);
  object->setCollidingGeometry(geom);
  scene->addSceneObject(object);

  // Update Camera position
  scene->getActiveCamera()->setPosition(0.0, 0.0, 10.0);
  scene->getActiveCamera()->setFocalPoint(0, 0, 0);

  // Device Client
  imstk::imstkNew<imstk::DummyClient>           client("DummyClient");
  imstk::imstkNew<imstk::SceneObjectController> controller(object, client);
  scene->addController(controller);

  // Run the simulation
  {
    // Setup a viewer to render in its own thread
    imstk::imstkNew<imstk::VTKViewer> viewer;
    viewer->setActiveScene(scene);

    // Setup a scene manager to advance the scene in its own thread
    imstk::imstkNew<imstk::SceneManager> sceneManager;
    sceneManager->setActiveScene(scene);
    double t = 0.0;

    imstk::connect<imstk::Event>(sceneManager, &imstk::SceneManager::postUpdate,
      [&](imstk::Event*)
      {
        t += sceneManager->getDt();
        client->setPosition(imstk::Vec3d(cos(t) * 10.0, sin(t) * 5.0, 0.0));
      });

    this->observeRigidBody(sceneManager, object, outputNode, outputTransformNode);

    imstk::imstkNew<imstk::SimulationManager> driver;
    driver->addModule(viewer);
    driver->addModule(sceneManager);

    // Add mouse and keyboard controls to the viewer
    {
      imstk::imstkNew<imstk::MouseSceneControl> mouseControl(viewer->getMouseDevice());
      mouseControl->setSceneManager(sceneManager);
      viewer->addControl(mouseControl);

      imstk::imstkNew<imstk::KeyboardSceneControl> keyControl(viewer->getKeyboardDevice());
      keyControl->setSceneManager(sceneManager);
      keyControl->setModuleDriver(driver);
      viewer->addControl(keyControl);
    }
    this->simulations[simName] = driver;
    driver->start();
  }

}

void vtkSlicerIMSTKLogic::observeRigidBody(std::shared_ptr<imstk::SceneManager> sceneManager, std::shared_ptr<imstk::SceneObject> object, vtkMRMLModelNode* outputNode, vtkMRMLLinearTransformNode* outputTransformNode)
{
  vtkSmartPointer<vtkPolyData> polyDataOutput = imstk::GeometryUtils::copyToVtkPolyData(std::dynamic_pointer_cast<imstk::SurfaceMesh>(object->getVisualGeometry()));
  outputNode->SetAndObservePolyData(polyDataOutput);
  outputNode->SetAndObserveTransformNodeID(outputTransformNode->GetID());

  imstk::connect<imstk::Event>(sceneManager, &imstk::SceneManager::postUpdate,
    [&](imstk::Event*)
    {
      auto test = std::dynamic_pointer_cast<imstk::SurfaceMesh>(object->getVisualGeometry());
      auto transform = test->getTransform();
      vtkNew<vtkMatrix4x4> matrix;
      for (int i = 0; i < 4; i++)
      {
        for (int j = 0; j < 4; j++)
        {
          matrix->SetElement(i, j, transform(i, j));
        }
      }
      outputTransformNode->SetMatrixTransformFromParent(matrix);
    });
}

void vtkSlicerIMSTKLogic::stopSimulation(std::string simName)
{
  auto simulation = this->simulations[simName];
  simulation->requestStatus(ModuleDriverStopped);
}
