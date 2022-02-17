
set(proj Libusb)

set(${proj}_SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj})
set(${proj}_INSTALL_DIR ${CMAKE_BINARY_DIR}/${proj}-install)

include(${iMSTK_SOURCE_DIR}/CMake/External/External_${proj}.cmake)
if(NOT SB_SECOND_PASS)
  return()
endif()

if(USE_SYSTEM_${proj})  # Variable name set in Libusb external project provided by iMSTK
  ExternalProject_Message(${proj} "Libusb_INCLUDE_DIR:${Libusb_INCLUDE_DIR}")
  ExternalProject_Message(${proj} "Libusb_LIBRARY_libusb-1.0-RELEASE:${Libusb_LIBRARY_libusb-1.0-RELEASE}")
  ExternalProject_Message(${proj} "Libusb_LIBRARY_libusb-1.0-DEBUG:${Libusb_LIBRARY_libusb-1.0-DEBUG}")

  mark_as_superbuild(
    VARS
      Libusb_INCLUDE_DIR:PATH
      Libusb_LIBRARY_libusb-1.0-RELEASE:FILEPATH
      Libusb_LIBRARY_libusb-1.0-DEBUG:FILEPATH
    PROJECTS
      iMSTK
    )
else()

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

  # Variable used in extension CMakeLists.txt to specify install rule
  mark_as_superbuild(${proj}_DLL_NAME:STRING)

  #-----------------------------------------------------------------------------
  # Launcher setting specific to build tree

  set(${proj}_LIBRARY_PATHS_LAUNCHER_BUILD ${${proj}_INSTALL_DIR}/${${proj}_DLL_DIR})
  mark_as_superbuild(
    VARS ${proj}_LIBRARY_PATHS_LAUNCHER_BUILD
    LABELS "LIBRARY_PATHS_LAUNCHER_BUILD"
    )
endif()

