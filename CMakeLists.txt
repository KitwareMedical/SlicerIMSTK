cmake_minimum_required(VERSION 3.13.4)

project(SlicerIMSTK)

#-----------------------------------------------------------------------------
# Extension meta-information
set(EXTENSION_HOMEPAGE "http://slicer.org/slicerWiki/index.php/Documentation/Nightly/Extensions/SlicerIMSTK")
set(EXTENSION_CATEGORY "Simulation")
set(EXTENSION_CONTRIBUTORS "Johan Andruejol (Kitware Inc.)")
set(EXTENSION_DESCRIPTION "SlicerIMSTK provides Slicer with an interface to the iMSTK simulation library.
For more information about iMSTK see https://gitlab.kitware.com/iMSTK/iMSTK")
set(EXTENSION_ICONURL "https://gitlab.kitware.com/iMSTK/iMSTK/blob/v1.0.0/Docs/source/media/logo.png")
set(EXTENSION_SCREENSHOTURLS "http://www.example.com/Slicer/Extensions/SlicerIMSTK/Screenshots/1.png")
set(EXTENSION_DEPENDS "NA") # Specified as a space separated string, a list or 'NA' if any
set(EXTENSION_BUILD_SUBDIRECTORY inner-build)

set(SUPERBUILD_TOPLEVEL_PROJECT inner)

#-----------------------------------------------------------------------------
# Extension dependencies
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})
mark_as_superbuild(Slicer_DIR)

find_package(Git REQUIRED)
mark_as_superbuild(GIT_EXECUTABLE)

#-----------------------------------------------------------------------------
set(_build_module_default ON)
if(DEFINED Slicer_SOURCE_DIR)
  set(_build_module_default OFF)
endif()

option(SlicerIMSTK_BUILD_CollisionSimulation "Enable the building of CollisionSimulation module." ${_build_module_default})
mark_as_superbuild(SlicerIMSTK_BUILD_CollisionSimulation)

option(SlicerIMSTK_BUILD_SimulationData "Enable the building of SimulationData module." ${_build_module_default})
mark_as_superbuild(SlicerIMSTK_BUILD_SimulationData)

#-----------------------------------------------------------------------------
# SuperBuild setup
option(${EXTENSION_NAME}_SUPERBUILD "Build ${EXTENSION_NAME} and the projects it depends on." ON)
mark_as_advanced(${EXTENSION_NAME}_SUPERBUILD)
if(${EXTENSION_NAME}_SUPERBUILD)
  include("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuildPrerequisites.cmake")
  include("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake")
  return()
endif()

#-----------------------------------------------------------------------------
# Extension modules
if(SlicerIMSTK_BUILD_CollisionSimulation)
  add_subdirectory(CollisionSimulation)
endif()
if(SlicerIMSTK_BUILD_SimulationData)
  add_subdirectory(SimulationData)
endif()

#-----------------------------------------------------------------------------
# Install FTD2XX
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  install(FILES ${FTD2XX_LIBRARY_PATHS_LAUNCHER_BUILD}/${FTD2XX_DLL_NAME}
    DESTINATION ${Slicer_THIRDPARTY_LIB_DIR}
    COMPONENT RuntimeLibraries
    )
  if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
    install(FILES ${FTD2XX_LIBRARY_PATHS_LAUNCHER_BUILD}/${FTD2XX_DLL_NAME}
      RENAME "ftd2xx.dll"
      DESTINATION ${Slicer_THIRDPARTY_LIB_DIR}
      COMPONENT RuntimeLibraries
      )
  endif()
endif()

#-----------------------------------------------------------------------------
# Install Libusb
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  install(FILES ${Libusb_LIBRARY_PATHS_LAUNCHER_BUILD}/${Libusb_DLL_NAME}
    DESTINATION ${Slicer_THIRDPARTY_LIB_DIR}
    COMPONENT RuntimeLibraries
    )
endif()

#-----------------------------------------------------------------------------
# Install OpenVR
install(FILES ${OpenVR_LIBRARY_PATHS_LAUNCHER_BUILD}/${OpenVR_DLL_NAME}
  DESTINATION ${Slicer_THIRDPARTY_LIB_DIR}
  COMPONENT RuntimeLibraries
  )

#-----------------------------------------------------------------------------
set(EXTENSION_CPACK_INSTALL_CMAKE_PROJECTS)
list(APPEND EXTENSION_CPACK_INSTALL_CMAKE_PROJECTS "${iMSTK_DIR};iMSTK;RuntimeLibraries;/")
list(APPEND EXTENSION_CPACK_INSTALL_CMAKE_PROJECTS "${iMSTK_DIR}/../Assimp-build;Assimp;Unspecified;/")
set(${EXTENSION_NAME}_CPACK_INSTALL_CMAKE_PROJECTS "${EXTENSION_CPACK_INSTALL_CMAKE_PROJECTS}" CACHE STRING "List of external projects to install" FORCE)

#-----------------------------------------------------------------------------
list(APPEND CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};${EXTENSION_NAME};ALL;/")
list(APPEND CPACK_INSTALL_CMAKE_PROJECTS "${${EXTENSION_NAME}_CPACK_INSTALL_CMAKE_PROJECTS}")
include(${Slicer_EXTENSION_GENERATE_CONFIG})
include(${Slicer_EXTENSION_CPACK})
