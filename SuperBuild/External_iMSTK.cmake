
set(proj iMSTK)

# Set dependency list
set(${proj}_DEPENDS
  )

# Include dependent projects if any
ExternalProject_Include_Dependencies(${proj} PROJECT_VAR proj)

if(${SUPERBUILD_TOPLEVEL_PROJECT}_USE_SYSTEM_${proj})
  message(FATAL_ERROR "Enabling ${SUPERBUILD_TOPLEVEL_PROJECT}_USE_SYSTEM_${proj} is not supported !")
endif()

# Sanity checks
if(DEFINED Foo_DIR AND NOT EXISTS ${Foo_DIR})
  message(FATAL_ERROR "Foo_DIR [${Foo_DIR}] variable is defined but corresponds to nonexistent directory")
endif()

if(NOT DEFINED ${proj}_DIR AND NOT ${SUPERBUILD_TOPLEVEL_PROJECT}_USE_SYSTEM_${proj})

  ExternalProject_SetIfNotDefined(
    ${SUPERBUILD_TOPLEVEL_PROJECT}_${proj}_GIT_REPOSITORY
    "https://gitlab.kitware.com/iMSTK/iMSTK.git"
    QUIET
    )

  ExternalProject_SetIfNotDefined(
    ${SUPERBUILD_TOPLEVEL_PROJECT}_${proj}_GIT_TAG
    "f04c77630ed6ab5ce5a0da5aa014879d7477be10"
    QUIET
    )

  set(${proj}_SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj})
  set(${proj}_BINARY_DIR ${CMAKE_BINARY_DIR}/${proj}-build)
  set(${proj}_RUNTIME_OUTPUT_DIRECTORY ${${proj}_BINARY_DIR}/bin)
  set(${proj}_LIBRARY_OUTPUT_DIRECTORY ${${proj}_BINARY_DIR}/lib)
  set(${proj}_ARCHIVE_OUTPUT_DIRECTORY ${${proj}_BINARY_DIR}/lib)

  ExternalProject_Add(${proj}
    ${${proj}_EP_ARGS}
    GIT_REPOSITORY "${${SUPERBUILD_TOPLEVEL_PROJECT}_${proj}_GIT_REPOSITORY}"
    GIT_TAG "${${SUPERBUILD_TOPLEVEL_PROJECT}_${proj}_GIT_TAG}"
    SOURCE_DIR ${${proj}_SOURCE_DIR}
    BINARY_DIR ${${proj}_BINARY_DIR}
    CMAKE_CACHE_ARGS
      # Compiler settings
      -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
      -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
      -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
      -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
      -DCMAKE_CXX_STANDARD:STRING=${CMAKE_CXX_STANDARD}
      -DCMAKE_CXX_STANDARD_REQUIRED:BOOL=${CMAKE_CXX_STANDARD_REQUIRED}
      -DCMAKE_CXX_EXTENSIONS:BOOL=${CMAKE_CXX_EXTENSIONS}
      # Output directories
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${${proj}_RUNTIME_OUTPUT_DIRECTORY}
      -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${${proj}_LIBRARY_OUTPUT_DIRECTORY}
      -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${${proj}_ARCHIVE_OUTPUT_DIRECTORY}
      # iMSTK vars
      -DiMSTK_SUPERBUILD:BOOL=ON
      -DUSE_SYSTEM_VTK:BOOL=ON
      -DBUILD_TESTING:BOOL=OFF
      # Dependencies
      -DVTK_DIR:PATH=${VTK_DIR}
    INSTALL_COMMAND ""
    DEPENDS
      ${${proj}_DEPENDS}
    )
  set(${proj}_DIR ${${proj}_BINARY_DIR}/InnerBuild)
  mark_as_superbuild(${proj}_DIR)

else()
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDS})
endif()

mark_as_superbuild(${proj}_DIR:PATH)
