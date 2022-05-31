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

# Set list of dependencies to ensure the custom application bundling this
# extension does NOT automatically collect the project list and attempt to
# build external projects associated with VTK modules enabled below.
set(SlicerIMSTK_EXTERNAL_PROJECT_DEPENDENCIES
  iMSTK
  )
if(DEFINED Slicer_SOURCE_DIR)
  # Extension is bundled in a custom application
  if(SlicerIMSTK_BUILD_ViewerVTK)
    list(APPEND SlicerIMSTK_EXTERNAL_PROJECT_DEPENDENCIES
      OpenVR
      )
  endif()
else()
  # Extension is build standalone against Slicer itself built
  # against VTK without the relevant modules enabled.
  if(SlicerIMSTK_BUILD_ViewerVTK)
    list(APPEND SlicerIMSTK_EXTERNAL_PROJECT_DEPENDENCIES
      vtkRenderingOpenVR
      )
  endif()
  # Add dependency on "tbb" only if not already built-in Slicer
  if(NOT Slicer_USE_TBB)
    list(APPEND EXTERNAL_PROJECT_ADDITIONAL_DIRS
      ${CMAKE_CURRENT_BINARY_DIR}/SuperBuild
      )
  endif()
endif()
message(STATUS "SlicerIMSTK_EXTERNAL_PROJECT_DEPENDENCIES:${SlicerIMSTK_EXTERNAL_PROJECT_DEPENDENCIES}")

# Download iMSTK sources so that External_*.cmake project provided by SlicerIMSTK
# can include the ones provided by iMSTK
include(${SlicerIMSTK_SOURCE_DIR}/FetchIMSTK.cmake)

# ... and update CMAKE_MODULE_PATH to ensure imstkAddExternalProject CMake module
# used in iMSTK external projects can be included.
set(CMAKE_MODULE_PATH
  ${iMSTK_SOURCE_DIR}/CMake
  ${iMSTK_SOURCE_DIR}/CMake/Utilities
  ${CMAKE_MODULE_PATH}
  )

if(NOT DEFINED Slicer_SOURCE_DIR)
  # Extension is built standalone

  # VTKExternalModule is required to configure these external projects:
  # - vtkRenderingVR
  # - vtkRenderingOpenVR

  include(${SlicerIMSTK_SOURCE_DIR}/FetchVTKExternalModule.cmake)

else()
  # Extension is bundled in a custom application

  if(SlicerIMSTK_BUILD_ViewerVTK)
    # Additional external project dependencies
    ExternalProject_Add_Dependencies(VTK
      DEPENDS
        OpenVR
      )
  endif()

  # Additional external project options
  if(SlicerIMSTK_BUILD_ViewerVTK)
    set(VTK_MODULE_ENABLE_VTK_RenderingExternal YES)
    set(VTK_MODULE_ENABLE_VTK_RenderingOpenVR YES)
  else()
    set(VTK_MODULE_ENABLE_VTK_RenderingExternal NO)
    set(VTK_MODULE_ENABLE_VTK_RenderingOpenVR NO)
  endif()
  mark_as_superbuild(
    VARS
      VTK_MODULE_ENABLE_VTK_RenderingExternal:STRING
      VTK_MODULE_ENABLE_VTK_RenderingOpenVR:STRING
    PROJECTS
      VTK
    )

endif()
