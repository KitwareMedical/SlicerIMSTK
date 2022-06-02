set(proj iMSTK)

# Set dependency list
set(${proj}_DEPENDS
  Assimp
  Eigen3
  g3log
  Libusb
  VegaFEM
  )
if(SlicerIMSTK_BUILD_ViewerVTK)
  list(APPEND ${proj}_DEPENDS
    OpenVR
    )
endif()

set(iMSTK_USE_OpenHaptics OFF)
if(WIN32)
  list(APPEND ${proj}_DEPENDS
    OpenHaptics
    )
  set(iMSTK_USE_OpenHaptics ON)
endif()

if(NOT SB_SECOND_PASS)
  mark_as_superbuild(
    VARS
      CMAKE_CXX_COMPILER:FILEPATH
      CMAKE_C_COMPILER:FILEPATH
      CMAKE_CXX_STANDARD:STRING
      CMAKE_CXX_STANDARD_REQUIRED:BOOL
      CMAKE_CXX_EXTENSIONS:BOOL
    PROJECTS
      ${${proj}_DEPENDS}
  )
endif()

set(_vtk_modules_depends
  )
if(SlicerIMSTK_BUILD_ViewerVTK)
  list(APPEND _vtk_modules_depends
    vtkRenderingExternal
    vtkRenderingOpenVR
    )
endif()
if(DEFINED Slicer_SOURCE_DIR)
  # Extension is bundled in a custom application
  list(APPEND ${proj}_DEPENDS
    tbb
    VTK
    )
else()
  # Extension is build standalone against Slicer itself built
  # against VTK without the relevant modules enabled.
  list(APPEND ${proj}_DEPENDS
    ${_vtk_modules_depends}
    )
  # Add dependency on "tbb" only if not already built-in Slicer
  if(NOT Slicer_USE_TBB)
    list(APPEND ${proj}_DEPENDS
      tbb
      )
  endif()
endif()

# Include dependent projects if any
ExternalProject_Include_Dependencies(${proj} PROJECT_VAR proj)

if(${SUPERBUILD_TOPLEVEL_PROJECT}_USE_SYSTEM_${proj})
  message(FATAL_ERROR "Enabling ${SUPERBUILD_TOPLEVEL_PROJECT}_USE_SYSTEM_${proj} is not supported !")
endif()

# Sanity checks
if(DEFINED ${proj}_DIR AND NOT EXISTS ${${proj}_DIR})
  message(FATAL_ERROR "${proj}_DIR [${${proj}_DIR}] variable is defined but corresponds to nonexistent directory")
endif()

if(NOT DEFINED ${proj}_DIR AND NOT ${SUPERBUILD_TOPLEVEL_PROJECT}_USE_SYSTEM_${proj})

  # Sanity checks
  if(NOT EXISTS "${${proj}_SOURCE_DIR}")
    message(FATAL_ERROR "${proj}_SOURCE_DIR [${${proj}_SOURCE_DIR}] variable is corresponds to nonexistent directory")
  endif()

  set(EP_SOURCE_DIR ${${proj}_SOURCE_DIR})
  set(EP_BINARY_DIR ${CMAKE_BINARY_DIR}/${proj}-build)

  set(EXTERNAL_PROJECT_OPTIONAL_CMAKE_CACHE_ARGS)
  foreach(_name IN LISTS _vtk_modules_depends)
    list(APPEND EXTERNAL_PROJECT_OPTIONAL_CMAKE_CACHE_ARGS
      -D${_name}_DIR:PATH=${${_name}_DIR}
      )
    set(_enabled "OFF")
    if(TARGET ${_name})
      set(_enabled "ON")
    endif()
    ExternalProject_Message(${proj} "${proj}[${_name}:${_enabled}]")
  endforeach()

  ExternalProject_Add(${proj}
    ${${proj}_EP_ARGS}
    SOURCE_DIR ${EP_SOURCE_DIR}
    BINARY_DIR ${EP_BINARY_DIR}
    CMAKE_CACHE_ARGS
      # Compiler settings
      -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
      -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
      -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
      -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
      -DCMAKE_CXX_STANDARD:STRING=${CMAKE_CXX_STANDARD}
      -DCMAKE_CXX_STANDARD_REQUIRED:BOOL=${CMAKE_CXX_STANDARD_REQUIRED}
      -DCMAKE_CXX_EXTENSIONS:BOOL=${CMAKE_CXX_EXTENSIONS}
      -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
      # Output directories
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${EP_BINARY_DIR}/bin
      -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${EP_BINARY_DIR}/lib
      -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${EP_BINARY_DIR}/lib
      # Options
      -DiMSTK_SUPERBUILD:BOOL=OFF
      -DiMSTK_BUILD_TESTING:BOOL=OFF
      -DiMSTK_BUILD_VISUAL_TESTING:BOOL=OFF
      -DiMSTK_BUILD_EXAMPLES:BOOL=OFF
      -DiMSTK_USE_MODEL_REDUCTION:BOOL=OFF
      -DiMSTK_ENABLE_AUDIO:BOOL=OFF
      -DiMSTK_USE_OpenHaptics:BOOL=${iMSTK_USE_OpenHaptics}
      -DiMSTK_USE_RENDERING_VTK:BOOL=${SlicerIMSTK_BUILD_ViewerVTK}
      # Dependencies
      -DTBB_DIR:PATH=${TBB_DIR}
      -DVTK_DIR:PATH=${VTK_DIR}
      ${EXTERNAL_PROJECT_OPTIONAL_CMAKE_CACHE_ARGS}
    INSTALL_COMMAND ""
    DEPENDS
      ${${proj}_DEPENDS}
    )
  set(${proj}_DIR ${EP_BINARY_DIR})

  #-----------------------------------------------------------------------------
  # Launcher setting specific to build tree

  # library paths
  # \todo: <CMAKE_CFG_INTDIR> doesn't work for now. Just add the types manually
  # See: https://issues.slicer.org/view.php?id=4682
  set(${proj}_LIBRARY_PATHS_LAUNCHER_BUILD
    ${${proj}_RUNTIME_OUTPUT_DIRECTORY}/Debug
    ${${proj}_RUNTIME_OUTPUT_DIRECTORY}/MinSizeRel
    ${${proj}_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo
    ${${proj}_RUNTIME_OUTPUT_DIRECTORY}/Release)
  mark_as_superbuild(
    VARS ${proj}_LIBRARY_PATHS_LAUNCHER_BUILD
    LABELS "LIBRARY_PATHS_LAUNCHER_BUILD"
  )

else()
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDS})
endif()

mark_as_superbuild(${proj}_DIR:PATH)

