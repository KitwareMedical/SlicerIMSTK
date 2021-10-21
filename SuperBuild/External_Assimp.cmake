
set(proj Assimp)

set(${proj}_SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj})
set(${proj}_BINARY_DIR ${CMAKE_BINARY_DIR}/${proj}-build)
set(${proj}_INSTALL_DIR ${CMAKE_BINARY_DIR}/${proj}-install)

include(${iMSTK_SOURCE_DIR}/CMake/External/External_${proj}.cmake)
if(NOT SB_SECOND_PASS)
  return()
endif()

set(${proj}_ROOT_DIR ${${proj}_INSTALL_DIR})
set(${proj}_LIB_DIR "lib")

ExternalProject_Message(${proj} "${proj}_ROOT_DIR:${${proj}_ROOT_DIR}")
ExternalProject_Message(${proj} "${proj}_LIB_DIR:${${proj}_LIB_DIR}")

mark_as_superbuild(
  VARS
    ${proj}_ROOT_DIR:PATH
    ${proj}_LIB_DIR:STRING
  PROJECTS
    iMSTK
  )

