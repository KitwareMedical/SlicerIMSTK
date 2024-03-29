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
# Options
include("${CMAKE_CURRENT_SOURCE_DIR}/SlicerIMSTKOptions.cmake")

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

add_subdirectory(IMSTK)

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
if(SlicerIMSTK_BUILD_ViewerVTK)
  install(FILES ${OpenVR_LIBRARY_PATHS_LAUNCHER_BUILD}/${OpenVR_DLL_NAME}
    DESTINATION ${Slicer_THIRDPARTY_LIB_DIR}
    COMPONENT RuntimeLibraries
    )
endif()

#-----------------------------------------------------------------------------
set(EXTENSION_CPACK_INSTALL_CMAKE_PROJECTS)
if(NOT APPLE)
  list(APPEND EXTENSION_CPACK_INSTALL_CMAKE_PROJECTS "${iMSTK_DIR}/../Assimp-build;Assimp;libassimp3.3.1;/")
  if(NOT Slicer_USE_TBB)
    include(SlicerFunctionInstallLibrary) # Required by SlicerBlockInstallTBB
    include(SlicerBlockInstallTBB)
  endif()
endif()
set(${EXTENSION_NAME}_CPACK_INSTALL_CMAKE_PROJECTS "${EXTENSION_CPACK_INSTALL_CMAKE_PROJECTS}" CACHE STRING "List of external projects to install" FORCE)

if(APPLE)
  set(EXTENSION_FIXUP_BUNDLE_LIBRARY_DIRECTORIES)
  list(APPEND EXTENSION_FIXUP_BUNDLE_LIBRARY_DIRECTORIES ${Assimp_ROOT_DIR})
  if(NOT Slicer_USE_TBB)
    list(APPEND EXTENSION_FIXUP_BUNDLE_LIBRARY_DIRECTORIES ${TBB_LIB_DIR})
  endif()
  set(${EXTENSION_NAME}_FIXUP_BUNDLE_LIBRARY_DIRECTORIES "${EXTENSION_FIXUP_BUNDLE_LIBRARY_DIRECTORIES}" CACHE STRING "List of directories to look up libraries to copy into the application package" FORCE)
endif()

#-----------------------------------------------------------------------------
list(APPEND CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};${EXTENSION_NAME};ALL;/")
list(APPEND CPACK_INSTALL_CMAKE_PROJECTS "${${EXTENSION_NAME}_CPACK_INSTALL_CMAKE_PROJECTS}")
include(${Slicer_EXTENSION_GENERATE_CONFIG})
include(${Slicer_EXTENSION_CPACK})
