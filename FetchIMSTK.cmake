
include(FetchContent)

set(proj iMSTK)
set(EP_SOURCE_DIR "${CMAKE_BINARY_DIR}/${proj}")
FetchContent_Populate(${proj}
  SOURCE_DIR     ${EP_SOURCE_DIR}
  GIT_REPOSITORY git://github.com/jcfr/iMSTK.git
  GIT_TAG        6997eab385ca2eac6c1f192663cde809810f220a  # update-build-system-to-streamline-application-integration
  QUIET
  )
message(STATUS "Remote - ${proj} [OK]")

set(iMSTK_SOURCE_DIR ${EP_SOURCE_DIR})
message(STATUS "Remote - iMSTK_SOURCE_DIR:${iMSTK_SOURCE_DIR}")

