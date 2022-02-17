
set(proj Assimp)

set(${proj}_SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj})
set(${proj}_BINARY_DIR ${CMAKE_BINARY_DIR}/${proj}-build)
set(${proj}_INSTALL_DIR ${CMAKE_BINARY_DIR}/${proj}-install)

set(ASSIMP_BIN_INSTALL_DIR ${Slicer_INSTALL_THIRDPARTY_BIN_DIR})
set(ASSIMP_LIB_INSTALL_DIR ${Slicer_INSTALL_THIRDPARTY_LIB_DIR})

mark_as_superbuild(
  VARS
    ASSIMP_BIN_INSTALL_DIR:STRING
    ASSIMP_LIB_INSTALL_DIR:STRING
  PROJECTS ${proj}
  )

include(${iMSTK_SOURCE_DIR}/CMake/External/External_${proj}.cmake)
if(NOT SB_SECOND_PASS)
  return()
endif()

set(${proj}_ROOT_DIR ${${proj}_INSTALL_DIR})
set(${proj}_LIB_DIR "${ASSIMP_LIB_INSTALL_DIR}")

ExternalProject_Message(${proj} "${proj}_ROOT_DIR:${${proj}_ROOT_DIR}")
ExternalProject_Message(${proj} "${proj}_LIB_DIR:${${proj}_LIB_DIR}")

mark_as_superbuild(
  VARS
    ${proj}_ROOT_DIR:PATH
    ${proj}_LIB_DIR:STRING
  PROJECTS
    iMSTK
  )

#-----------------------------------------------------------------------------
# Launcher setting specific to build tree

set(${proj}_LIBRARY_PATHS_LAUNCHER_BUILD ${${proj}_INSTALL_DIR}/${ASSIMP_LIB_INSTALL_DIR})
mark_as_superbuild(
  VARS ${proj}_LIBRARY_PATHS_LAUNCHER_BUILD
  LABELS "LIBRARY_PATHS_LAUNCHER_BUILD"
  )

