
include(FetchContent)

set(proj iMSTK)
set(EP_SOURCE_DIR "${CMAKE_BINARY_DIR}/${proj}")
FetchContent_Populate(${proj}
  SOURCE_DIR     ${EP_SOURCE_DIR}
  GIT_REPOSITORY ${EP_GIT_PROTOCOL}://github.com/Kitware/iMSTK.git
  GIT_TAG        beb833499c4bbb705311d3654853c8af158112a6
  QUIET
  )
message(STATUS "Remote - ${proj} [OK]")

set(iMSTK_SOURCE_DIR ${EP_SOURCE_DIR})
message(STATUS "Remote - iMSTK_SOURCE_DIR:${iMSTK_SOURCE_DIR}")

