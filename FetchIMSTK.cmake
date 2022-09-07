
include(FetchContent)

if(NOT DEFINED iMSTK_SOURCE_DIR)
  set(proj iMSTK)
  set(EP_SOURCE_DIR "${CMAKE_BINARY_DIR}/${proj}")
  FetchContent_Populate(${proj}
    SOURCE_DIR     ${EP_SOURCE_DIR}
    GIT_REPOSITORY ${EP_GIT_PROTOCOL}://github.com/KitwareMedical/iMSTK.git
    GIT_TAG        27beeb520240ecc1bd9e21bdfe9f2bee2a206c9a  # slicerimstk-v6.0.0-2022-08-18-531dfe
    QUIET
    )
  message(STATUS "Remote - ${proj} [OK]")

  set(iMSTK_SOURCE_DIR ${EP_SOURCE_DIR})
endif()
message(STATUS "Remote - iMSTK_SOURCE_DIR:${iMSTK_SOURCE_DIR}")

