
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

if(NOT DEFINED SlicerIMSTK_BUILD_HapticsDeviceClient)
  if(WIN32)
    set(_default OFF)
    message(STATUS "SlicerIMSTK_BUILD_HapticsDeviceClient is not defined. Defaulting to '${_default}'")
    option(SlicerIMSTK_BUILD_HapticsDeviceClient "Build iMSTK HapticsDeviceClient (OpenHaptics)" ${_default})
  else()
    set(SlicerIMSTK_BUILD_HapticsDeviceClient OFF)
  endif()
endif()
if(SlicerIMSTK_BUILD_HapticsDeviceClient AND NOT WIN32)
  message(FATAL_ERROR "Setting SlicerIMSTK_BUILD_HapticsDeviceClient to ON is only supported on Windows")
endif()
mark_as_superbuild(SlicerIMSTK_BUILD_HapticsDeviceClient)
