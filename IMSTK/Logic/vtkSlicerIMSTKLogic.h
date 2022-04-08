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

#ifndef __vtkSlicerIMSTKLogic_h
#define __vtkSlicerIMSTKLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes

// iMSTK includes

// STD includes
#include <cstdlib>
#include <memory>
#include <map>


#include "vtkSlicerIMSTKModuleLogicExport.h"

class vtkMRMLModelNode;
class vtkMRMLLinearTransformNode;


namespace imstk
{
  class SceneManager;
  class SceneObject;
  class SimulationManager;
}


/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_IMSTK_MODULE_LOGIC_EXPORT vtkSlicerIMSTKLogic :
  public vtkSlicerModuleLogic
{
public:

  static vtkSlicerIMSTKLogic *New();
  vtkTypeMacro(vtkSlicerIMSTKLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  void runObjectCtrlDummyClientExample(std::string simName, vtkMRMLModelNode* inputNode, vtkMRMLModelNode* outputNode, vtkMRMLLinearTransformNode* outputTransformNode);
  void observeRigidBody(std::shared_ptr<imstk::SceneManager> sceneManager, std::shared_ptr<imstk::SceneObject> object, vtkMRMLModelNode* outputNode, vtkMRMLLinearTransformNode* outputTransformNode);
  void runHapticDeviceExample(std::string simName, std::string deviceName, vtkMRMLLinearTransformNode* outputTransformNode);
  void stopSimulation(std::string simName);

protected:
  vtkSlicerIMSTKLogic();
  ~vtkSlicerIMSTKLogic() override;

  void SetMRMLSceneInternal(vtkMRMLScene* newScene) override;
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  void RegisterNodes() override;
  void UpdateFromMRMLScene() override;
  void OnMRMLSceneNodeAdded(vtkMRMLNode* node) override;
  void OnMRMLSceneNodeRemoved(vtkMRMLNode* node) override;
private:

  vtkSlicerIMSTKLogic(const vtkSlicerIMSTKLogic&); // Not implemented
  void operator=(const vtkSlicerIMSTKLogic&); // Not implemented
  std::map<std::string,std::shared_ptr<imstk::SimulationManager>> simulations;
};

#endif
