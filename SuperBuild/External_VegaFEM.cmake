
set(proj VegaFEM)

set(${proj}_SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj})
set(${proj}_BINARY_DIR ${CMAKE_BINARY_DIR}/${proj}-build)
set(${proj}_INSTALL_DIR ${CMAKE_BINARY_DIR}/${proj}-install)

include(${iMSTK_SOURCE_DIR}/CMake/External/External_${proj}.cmake)
if(NOT SB_SECOND_PASS)
  return()
endif()

set(${proj}_DIR ${${proj}_INSTALL_DIR}/lib/cmake/VegaFEM)

ExternalProject_Message(${proj} "${proj}_DIR:${${proj}_DIR}")

mark_as_superbuild(
  VARS
    ${proj}_DIR:PATH
  PROJECTS
    iMSTK
  )

