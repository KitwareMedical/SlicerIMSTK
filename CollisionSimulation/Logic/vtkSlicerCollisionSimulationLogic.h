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

// .NAME vtkSlicerCollisionSimulationLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerCollisionSimulationLogic_h
#define __vtkSlicerCollisionSimulationLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"
#include <map>

// MRML includes
class vtkMRMLCommandLineModuleNode;
class vtkMRMLModelNode;
class vtkMRMLLinearTransformNode;

// VTK includes
class vtkUnstructuredGrid;

// iMSTK includes
#include <imstkSimulationManager.h>
#include <imstkMeshIO.h>
#include <imstkOneToOneMap.h>
#include <imstkPbdModel.h>
#include <imstkPbdObject.h>
#include <imstkPbdSolver.h>
#include <imstkDummyClient.h>

// STD includes
#include <cstdlib>
#include <memory>

// Module includes
#include "vtkSlicerCollisionSimulationModuleLogicExport.h"

/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_COLLISIONSIMULATION_MODULE_LOGIC_EXPORT vtkSlicerCollisionSimulationLogic :
  public vtkSlicerModuleLogic
{
public:

  static vtkSlicerCollisionSimulationLogic *New();
  vtkTypeMacro(vtkSlicerCollisionSimulationLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);

  /// Write the node to a temporary folder and return the path
  std::string WriteNodeToTemporaryDirectory(vtkMRMLNode* node);

  /// imstk::SimulationManager
  ///
  /// Create the SDK if it doesn't exist already. The logic owns the SDK.
  void CreateSDK(bool enableVR = false, bool disableRendering = true);

  /// imstk::SimulationManager
  ///
  /// Return the current SDK. If none exist, one is created with the default
  /// parameters.
  /// \sa CreateSDK
  std::shared_ptr<imstk::SimulationManager> GetSDK();


  /// imstk::Scene
  ///
  /// Create a scene with the given name. It will be added to the SDK.
  /// By default, it will be made the active scene.
  /// \sa GetSDK
  void CreateScene(const std::string& name, bool makeActive = true);

  /// imstk::Scene
  ///
  /// Set the active scene on the SDK.
  void SetActiveScene(const std::string& name, bool unloadPrevious = true);

  /// imstk collision
  ///
  /// Add collision interaction between the two objects.
  /// They must be in the scene.
  void AddCollisionInteraction(
    const std::string& obj1, const std::string& obj2);
  void AddCollisionInteraction(
    const std::string& name,
    const std::string& obj1, const std::string& obj2);

  /// imstk immovable object
  ///
  /// Add an immovable object to the given scene. The model must have an
  /// unstructured grid.
  void AddImmovableObject(
    const std::string& name,
    vtkMRMLModelNode* modelNode,
    double dt);
  /// Add an immovable object to the active scene. The model must have an
  /// unstructured grid.
  void AddImmovableObject(
    vtkMRMLModelNode* modelNode,
    double dt);

  /// Create a parameter node with default values for immovable objects.
  /// The returned node is NOT added to the scene.
  /// You are responsible for deleting the returned node.
  vtkMRMLCommandLineModuleNode* DefaultImmovableObjectParameterNode();

  /// imstk deformable object
  ///
  /// Add a deformable object to the given scene. The model must have an
  /// unstructured grid.
  void AddDeformableObject(
    const std::string& name,
    vtkMRMLModelNode* modelNode,
    double gravity, double stiffness, double dt, double youngs, double poisson);
  /// Add an deformable object to the active scene. The model must have an
  /// unstructured grid.
  void AddDeformableObject(
    vtkMRMLModelNode* modelNode,
    double gravity, double stiffness, double dt, double youngs, double poisson);

  /// Create a parameter node with default values for deformable objects.
  /// The returned node is NOT added to the scene.
  /// You are responsible for deleting the returned node.
  vtkMRMLCommandLineModuleNode* DefaultDeformableObjectParameterNode();

  /// Mesh updates methods
  ///
  /// Update the given polydata points from the given object name in the
  /// active scene
  void UpdateMeshPointsFromObject(
    const std::string& objectName, vtkUnstructuredGrid* mesh);
  void UpdateMeshPointsFromObject(
    const std::string& objectName, vtkMRMLModelNode* mesh);

  //Add a controller to an object
  void AttachTransformController(const std::string& objectName, vtkMRMLLinearTransformNode* transform);

  //Add a controller to an object
  void AttachTransformController(const std::string& name, const std::string& objectName, vtkMRMLLinearTransformNode* transform);

  void UpdateControllerFromTransform(vtkMRMLLinearTransformNode* transform);

  

protected:
  vtkSlicerCollisionSimulationLogic();
  virtual ~vtkSlicerCollisionSimulationLogic();

  void AddImmovableObject(
    std::shared_ptr<imstk::Scene> scene,
    vtkMRMLModelNode* modelNode,
    double dt);
  void AddCollisionInteraction(
    std::shared_ptr<imstk::Scene> scene,
    const std::string& obj1,
    const std::string& obj2);
  void AddDeformableObject(
    std::shared_ptr<imstk::Scene> scene,
    vtkMRMLModelNode* modelNode,
    double gravity, double stiffness, double dt, double youngs, double poisson);

private:
  vtkSlicerCollisionSimulationLogic(const vtkSlicerCollisionSimulationLogic&); // Not implemented
  void operator=(const vtkSlicerCollisionSimulationLogic&); // Not implemented

  std::shared_ptr<imstk::SimulationManager> SDK;
  std::map < std::string, std::shared_ptr<imstk::PbdObject>> m_Objects;
  std::map < std::string, std::shared_ptr<imstk::SurfaceMesh>> m_Meshes;
  std::map < std::string, std::shared_ptr<imstk::PbdSolver>> m_Solvers;
  std::map < std::string, std::shared_ptr<imstk::DummyClient>> m_TransformClients;


};

#endif
