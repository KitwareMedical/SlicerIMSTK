set(proj OpenVR)

set(${proj}_SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj})
set(${proj}_INSTALL_DIR ${${proj}_SOURCE_DIR})

include(${iMSTK_SOURCE_DIR}/CMake/External/External_${proj}.cmake)
if(NOT SB_SECOND_PASS)
  return()
endif()

set(OpenVR_INCLUDE_DIR "${${proj}_INSTALL_DIR}/${OpenVR_INC_DIR}")
set(OpenVR_LIBRARY "${${proj}_INSTALL_DIR}/${OpenVR_LIB_DIR}/${OpenVR_LIB_NAME}")

ExternalProject_Message(${proj} "OpenVR_INCLUDE_DIR:${OpenVR_INCLUDE_DIR}")
ExternalProject_Message(${proj} "OpenVR_LIBRARY:${OpenVR_LIBRARY}")

mark_as_superbuild(
  VARS
    ${proj}_INCLUDE_DIR:PATH
    ${proj}_LIBRARY:FILEPATH
  PROJECTS
    iMSTK
    VTK
  )

set(${proj}_ROOT_DIR ${${proj}_INSTALL_DIR})

ExternalProject_Message(${proj} "${proj}_ROOT_DIR:${${proj}_ROOT_DIR}")
ExternalProject_Message(${proj} "${proj}_LIB_DIR:${${proj}_LIB_DIR}")
ExternalProject_Message(${proj} "${proj}_INC_DIR:${${proj}_INC_DIR}")

mark_as_superbuild(
  VARS
    ${proj}_ROOT_DIR:PATH
    ${proj}_LIB_DIR:STRING
    ${proj}_INC_DIR:STRING
  PROJECTS
    iMSTK
  )

# Variable used in extension CMakeLists.txt to specify install rule
mark_as_superbuild(${proj}_DLL_NAME:STRING)

#-----------------------------------------------------------------------------
# Launcher setting specific to build tree

set(${proj}_LIBRARY_PATHS_LAUNCHER_BUILD ${${proj}_INSTALL_DIR}/${${proj}_DLL_DIR})
mark_as_superbuild(
  VARS ${proj}_LIBRARY_PATHS_LAUNCHER_BUILD
  LABELS "LIBRARY_PATHS_LAUNCHER_BUILD"
  )
