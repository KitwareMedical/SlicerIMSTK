
include(FetchContent)

set(proj iMSTK)
set(EP_SOURCE_DIR "${CMAKE_BINARY_DIR}/${proj}")
FetchContent_Populate(${proj}
  SOURCE_DIR     ${EP_SOURCE_DIR}
  GIT_REPOSITORY git://github.com/jcfr/iMSTK.git
  GIT_TAG        210c0b4295ba5bcd6c4960f26437d6b74d4e7646  # update-build-system-to-streamline-application-integration
  QUIET
  )
message(STATUS "Remote - ${proj} [OK]")

set(iMSTK_SOURCE_DIR ${EP_SOURCE_DIR})
message(STATUS "Remote - iMSTK_SOURCE_DIR:${iMSTK_SOURCE_DIR}")

