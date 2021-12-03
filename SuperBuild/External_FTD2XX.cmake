
set(proj FTD2XX)

set(${proj}_SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj})
set(${proj}_INSTALL_DIR ${CMAKE_BINARY_DIR}/${proj}-install)

include(${iMSTK_SOURCE_DIR}/CMake/External/External_${proj}.cmake)

# The following variables are required by the Findlibftd2xx CMake module provided
# by the LibNiFalcon project and directly used in the LibNiFalcon project:
# - LIBFTD2XX_INCLUDE_DIR
# - LIBFTD2XX_LIBRARY
# Note that the variables FTD2XX_LIB_DIR and FTD2XX_LIB_NAME used below
# are set in the FTD2XX external project provided by the iMSTK project
# and included above.
set(LIBFTD2XX_INCLUDE_DIR "${${proj}_INSTALL_DIR}/include/ftd2xx")
set(LIBFTD2XX_LIBRARY "${${proj}_INSTALL_DIR}/${FTD2XX_LIB_DIR}/${FTD2XX_LIB_NAME}")

if(NOT SB_SECOND_PASS)
  return()
endif()

set(${proj}_ROOT_DIR ${${proj}_INSTALL_DIR})

ExternalProject_Message(${proj} "${proj}_ROOT_DIR:${${proj}_ROOT_DIR}")
ExternalProject_Message(${proj} "${proj}_LIB_DIR:${${proj}_LIB_DIR}")

ExternalProject_Message(${proj} "LIBFTD2XX_INCLUDE_DIR:${LIBFTD2XX_INCLUDE_DIR}")
ExternalProject_Message(${proj} "LIBFTD2XX_LIBRARY:${LIBFTD2XX_LIBRARY}")

mark_as_superbuild(
  VARS
    ${proj}_ROOT_DIR:PATH
    ${proj}_LIB_DIR:STRING
    LIBFTD2XX_INCLUDE_DIR:PATH
    LIBFTD2XX_LIBRARY:FILEPATH
  PROJECTS
    iMSTK
  )

