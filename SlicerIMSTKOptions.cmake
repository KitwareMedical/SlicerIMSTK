
if(NOT DEFINED SlicerIMSTK_BUILD_ViewerVTK)
  # Since iMSTK ViewerVTK expects OpenVR library that is available only on Linux and Windows,
  # default value is initialized to OFF for macOS.
  # See https://gitlab.kitware.com/iMSTK/iMSTK/-/issues/432
  set(_default ON)
  if(APPLE)
    set(_default OFF)
  endif()
  message(STATUS "SlicerIMSTK_BUILD_ViewerVTK is not defined. Defaulting to '${_default}'")
  option(SlicerIMSTK_BUILD_ViewerVTK "Build iMSTK ViewerVTK" ${_default})
endif()
mark_as_superbuild(SlicerIMSTK_BUILD_ViewerVTK)
