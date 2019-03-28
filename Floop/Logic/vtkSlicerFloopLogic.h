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

// .NAME vtkSlicerFloopLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerFloopLogic_h
#define __vtkSlicerFloopLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes

// STD includes
#include <cstdlib>

#include "vtkSlicerFloopModuleLogicExport.h"


/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_FLOOP_MODULE_LOGIC_EXPORT vtkSlicerFloopLogic :
  public vtkSlicerModuleLogic
{
public:

  static vtkSlicerFloopLogic *New();
  vtkTypeMacro(vtkSlicerFloopLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkSlicerFloopLogic();
  virtual ~vtkSlicerFloopLogic();

private:

  vtkSlicerFloopLogic(const vtkSlicerFloopLogic&); // Not implemented
  void operator=(const vtkSlicerFloopLogic&); // Not implemented
};

#endif