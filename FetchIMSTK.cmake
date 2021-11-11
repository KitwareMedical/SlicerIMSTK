
include(FetchContent)

set(proj iMSTK)
set(EP_SOURCE_DIR "${CMAKE_BINARY_DIR}/${proj}")
FetchContent_Populate(${proj}
  SOURCE_DIR     ${EP_SOURCE_DIR}
  GIT_REPOSITORY git://github.com/jcfr/iMSTK.git
  GIT_TAG        0ffa63e9b6e44f986b385bda4aaec4a17956e842  # update-build-system-to-streamline-application-integration
  QUIET
  )
message(STATUS "Remote - ${proj} [OK]")

set(iMSTK_SOURCE_DIR ${EP_SOURCE_DIR})
message(STATUS "Remote - iMSTK_SOURCE_DIR:${iMSTK_SOURCE_DIR}")

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

  set(SlicerIMSTK_EXTERNAL_PROJECT_DEPENDENCIES
    Assimp
    g3log
    iMSTK
    LibNiFalcon
    Libusb
    OpenVR
    VegaFEM
    )
  message(STATUS "Remote - ${proj} explicitly set SlicerIMSTK_EXTERNAL_PROJECT_DEPENDENCIES to  [${SlicerIMSTK_EXTERNAL_PROJECT_DEPENDENCIES}]")

endif()
