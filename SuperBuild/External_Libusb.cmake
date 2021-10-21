
set(proj Libusb)

if(NOT WIN32)
  set(Slicer_USE_SYSTEM_${proj} TRUE)
endif()

# Set dependency list
set(${proj}_DEPENDENCIES "")

# Sanity checks
if(DEFINED ${proj}_ROOT_DIR AND NOT EXISTS ${${proj}_ROOT_DIR})
  message(FATAL_ERROR "${proj}_ROOT_DIR variable is defined but corresponds to nonexistent directory")
endif()

if(Slicer_USE_SYSTEM_${proj})
  unset(Libusb_INCLUDE_DIR CACHE)
  unset(Libusb_LIBRARY_libusb-1.0-RELEASE CACHE)
  unset(Libusb_LIBRARY_libusb-1.0-DEBUG CACHE)

  find_package(${proj} REQUIRED)
endif()

# Include dependent projects if any
ExternalProject_Include_Dependencies(${proj} PROJECT_VAR proj DEPENDS_VAR ${proj}_DEPENDENCIES)

if((NOT DEFINED ${proj}_ROOT_DIR
   OR NOT DEFINED ${proj}_LIB_DIR) AND NOT Slicer_USE_SYSTEM_${proj})

  set(EP_SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj})
  set(EP_INSTALL_DIR ${CMAKE_BINARY_DIR}/${proj}-install)

  # Set install commands
  set(libusb_libdir "MS32")
  if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
    set(libusb_libdir "MS64")
  endif()

  set(copy_libusb_headers_command
    ${CMAKE_COMMAND} -E copy_directory
    ${EP_SOURCE_DIR}/include
    ${EP_INSTALL_DIR}/include
    )
  set(copy_libusb_lib_command
    ${CMAKE_COMMAND} -E copy
    ${EP_SOURCE_DIR}/${libusb_libdir}/dll/libusb-1.0.lib
    ${EP_INSTALL_DIR}/lib/libusb-1.0.lib
    )
  set(copy_libusb_dll_command
    ${CMAKE_COMMAND} -E copy
    ${EP_SOURCE_DIR}/${libusb_libdir}/dll/libusb-1.0.dll
    ${EP_INSTALL_DIR}/bin/
    )

  ExternalProject_Add(${proj}
    ${${proj}_EP_ARGS}
    URL http://downloads.sourceforge.net/project/libusb/libusb-1.0/libusb-1.0.21/libusb-1.0.21.7z https://data.kitware.com/api/v1/item/59cbcefd8d777f7d33e9d9d7/download
    URL_MD5 7fbcf5580b8ffc88f3af6eddd638de9f
    DOWNLOAD_DIR ${CMAKE_BINARY_DIR}
    SOURCE_DIR ${EP_SOURCE_DIR}
    BINARY_DIR ${EP_BINARY_DIR}
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND
      COMMAND ${copy_libusb_headers_command}
      COMMAND ${copy_libusb_lib_command}
      COMMAND ${copy_libusb_dll_command}
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )

  # TODO: ExternalProject_GenerateProjectDescription_Step

  set(${proj}_ROOT_DIR ${EP_INSTALL_DIR})
  set(${proj}_LIB_DIR "lib")

  #-----------------------------------------------------------------------------
  # Launcher setting specific to build tree

  # NA

else()
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDENCIES})
endif()

if(Slicer_USE_SYSTEM_${proj})
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
  ExternalProject_Message(${proj} "${proj}_ROOT_DIR:${${proj}_ROOT_DIR}")
  ExternalProject_Message(${proj} "${proj}_LIB_DIR:${${proj}_LIB_DIR}")

  mark_as_superbuild(
    VARS
      ${proj}_ROOT_DIR:PATH
      ${proj}_LIB_DIR:STRING
    PROJECTS
      iMSTK
    )
endif()

