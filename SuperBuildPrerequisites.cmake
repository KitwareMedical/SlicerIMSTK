if(DEFINED slicersources_SOURCE_DIR AND NOT DEFINED Slicer_SOURCE_DIR)
  # Explicitly setting "Slicer_SOURCE_DIR" when only "slicersources_SOURCE_DIR"
  # is defined is required to successfully complete configuration in an empty
  # build directory
  #
  # Indeed, in that case, Slicer sources have been downloaded by they have not been
  # added using "add_subdirectory()" and the variable "Slicer_SOURCE_DIR" is not yet in
  # in the CACHE.
  set(Slicer_SOURCE_DIR ${slicersources_SOURCE_DIR})
endif()

# List of <external-project> dependencies used
# (1) to update "<extension-name>_EXTERNAL_PROJECT_DEPENDENCIES" below
# (2) to setup associated "External_<external-project>.cmake"
set(iMSTK_EXTERNAL_PROJECT_DEPENDENCIES
  Assimp
  Eigen3
  g3log
  LibNiFalcon
  Libusb
  OpenVR
  VegaFEM
  )

if(DEFINED Slicer_SOURCE_DIR)
  # Extension is bundled in a custom application

  # Explicit list of dependencies ensuring the custom application bundling this
  # extension does NOT attempt to build external projects associated with VTK modules
  # enabled below.
  set(SlicerIMSTK_EXTERNAL_PROJECT_DEPENDENCIES
    ${iMSTK_EXTERNAL_PROJECT_DEPENDENCIES}
    iMSTK
    )
  message(STATUS "SlicerIMSTK_EXTERNAL_PROJECT_DEPENDENCIES:${SlicerIMSTK_EXTERNAL_PROJECT_DEPENDENCIES}")
endif()

if(NOT DEFINED Slicer_SOURCE_DIR)
  # If extension is built standalone, VTKExternalModule is required
  # to configure vtkRenderingExternal and vtkRenderingOpenVR external
  # projects.
  include(${SlicerIMSTK_SOURCE_DIR}/FetchVTKExternalModule.cmake)
endif()

include(${SlicerIMSTK_SOURCE_DIR}/FetchIMSTK.cmake)
set(CMAKE_MODULE_PATH
  ${iMSTK_SOURCE_DIR}/CMake
  ${iMSTK_SOURCE_DIR}/CMake/Utilities
  ${CMAKE_MODULE_PATH}
  )

if(DEFINED Slicer_SOURCE_DIR)
  # Extension is bundled in a custom application

  # Additional external project dependencies
  ExternalProject_Add_Dependencies(VTK
    DEPENDS
      OpenVR
    )

  # Additional external project options
  set(VTK_MODULE_ENABLE_VTK_RenderingExternal YES)
  mark_as_superbuild(
    VARS
      VTK_MODULE_ENABLE_VTK_RenderingExternal:STRING
    PROJECTS
      VTK
    )

endif()
