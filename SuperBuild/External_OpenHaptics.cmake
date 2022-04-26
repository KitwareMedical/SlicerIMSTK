set(proj OpenHaptics)

set(${proj}_SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj})
set(${proj}_BINARY_DIR ${CMAKE_BINARY_DIR}/${proj}-build)
set(${proj}_INSTALL_DIR ${CMAKE_BINARY_DIR}/${proj}-install)


include(${iMSTK_SOURCE_DIR}/CMake/External/External_${proj}.cmake)
if(NOT SB_SECOND_PASS)
  return()
endif()

# HACK - workaround for these not being available by build time
file(MAKE_DIRECTORY ${${proj}_INSTALL_DIR}/lib)
file(MAKE_DIRECTORY ${${proj}_INSTALL_DIR}/bin)

set(${proj}_ROOT_DIR ${${proj}_INSTALL_DIR})
set(${proj}SDK_ROOT_DIR ${${proj}_INSTALL_DIR})
set(${proj}_LIB_DIR "lib")

ExternalProject_Message(${proj} "${proj}_ROOT_DIR:${${proj}_ROOT_DIR}")

mark_as_superbuild(
  VARS
    ${proj}_ROOT_DIR:PATH
    ${proj}SDK_ROOT_DIR:PATH
    ${proj}_LIB_DIR:STRING
  PROJECTS
    iMSTK
    Slicer
  )