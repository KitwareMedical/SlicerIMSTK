
include(FetchContent)

set(proj iMSTK)
set(EP_SOURCE_DIR "${CMAKE_BINARY_DIR}/${proj}")
FetchContent_Populate(${proj}
  SOURCE_DIR     ${EP_SOURCE_DIR}
  GIT_REPOSITORY ${EP_GIT_PROTOCOL}://github.com/KitwareMedical/iMSTK.git
  GIT_TAG        c1e1f758c03b051edf27fa9223e5df5a7404d938  # slicerimstk-v5.0.0-2022-05-26-beb83349
  QUIET
  )
message(STATUS "Remote - ${proj} [OK]")

set(iMSTK_SOURCE_DIR ${EP_SOURCE_DIR})
message(STATUS "Remote - iMSTK_SOURCE_DIR:${iMSTK_SOURCE_DIR}")

