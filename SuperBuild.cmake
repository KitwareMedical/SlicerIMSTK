#------------------------------------------------------------------------------
# Include remote modules
#------------------------------------------------------------------------------

include(FetchContent)

# VTKExternalModule
set(proj VTKExternalModule)
set(EP_SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj})
FetchContent_Populate(${proj}
  QUIET
  GIT_REPOSITORY ${EP_GIT_PROTOCOL}://github.com/KitwareMedical/VTKExternalModule
  GIT_TAG        3bae71e5eba073e589810a8bef947d65c90a2174
  SOURCE_DIR ${EP_SOURCE_DIR}
  )
message(STATUS "Remote - ${proj} [OK]")

set(VTKExternalModule_SOURCE_DIR ${EP_SOURCE_DIR})

# iMSTK
set(proj "iMSTK")
set(${proj}_SOURCE_DIR "${CMAKE_BINARY_DIR}/${proj}")
FetchContent_Populate(${proj}
  SOURCE_DIR     ${${proj}_SOURCE_DIR}
  GIT_REPOSITORY git://github.com/jcfr/iMSTK.git
  GIT_TAG        0ffa63e9b6e44f986b385bda4aaec4a17956e842  # update-build-system-to-streamline-application-integration
  GIT_PROGRESS   1
  QUIET
  )
message(STATUS "Remote - ${proj} [OK]")

set(CMAKE_MODULE_PATH
  ${iMSTK_SOURCE_DIR}/CMake
  ${iMSTK_SOURCE_DIR}/CMake/Utilities
  ${CMAKE_MODULE_PATH}
  )

#-----------------------------------------------------------------------------
# External project common settings
#-----------------------------------------------------------------------------

set(ep_common_c_flags "${CMAKE_C_FLAGS_INIT} ${ADDITIONAL_C_FLAGS}")
set(ep_common_cxx_flags "${CMAKE_CXX_FLAGS_INIT} ${ADDITIONAL_CXX_FLAGS}")

#-----------------------------------------------------------------------------
# Top-level "external" project
#-----------------------------------------------------------------------------

# Extension dependencies
foreach(dep ${EXTENSION_DEPENDS})
  mark_as_superbuild(${dep}_DIR)
endforeach()

set(proj ${SUPERBUILD_TOPLEVEL_PROJECT})

# Project dependencies
set(${proj}_DEPENDS
   iMSTK
   )

ExternalProject_Include_Dependencies(${proj}
  PROJECT_VAR proj
  SUPERBUILD_VAR ${EXTENSION_NAME}_SUPERBUILD
  )

ExternalProject_Add(${proj}
  ${${proj}_EP_ARGS}
  DOWNLOAD_COMMAND ""
  INSTALL_COMMAND ""
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
  BINARY_DIR ${EXTENSION_BUILD_SUBDIRECTORY}
  CMAKE_CACHE_ARGS
    # Compiler settings
    -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
    -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
    -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
    -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
    -DCMAKE_CXX_STANDARD:STRING=${CMAKE_CXX_STANDARD}
    -DCMAKE_CXX_STANDARD_REQUIRED:BOOL=${CMAKE_CXX_STANDARD_REQUIRED}
    -DCMAKE_CXX_EXTENSIONS:BOOL=${CMAKE_CXX_EXTENSIONS}
    # Output directories
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
    # Superbuild
    -D${EXTENSION_NAME}_SUPERBUILD:BOOL=OFF
    -DEXTENSION_SUPERBUILD_BINARY_DIR:PATH=${${EXTENSION_NAME}_BINARY_DIR}
  DEPENDS
    ${${proj}_DEPENDS}
  )

ExternalProject_AlwaysConfigure(${proj})
