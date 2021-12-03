
set(proj PThreads)

set(${proj}_SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj})
set(${proj}_BINARY_DIR ${CMAKE_BINARY_DIR}/${proj}-build)
set(${proj}_INSTALL_DIR ${CMAKE_BINARY_DIR}/${proj}-install)

include(${iMSTK_SOURCE_DIR}/CMake/External/External_${proj}.cmake)

# The following variables are required by the FindPThreads CMake module provided
# by VegaFEM project and directly used in the VegaFEM project:
# - PTHREAD_INCLUDE_DIR
# - PTHREAD_RELEASE_LIBRARY
# - PTHREAD_DEBUG_LIBRARY
set(PTHREAD_INCLUDE_DIR "${${proj}_INSTALL_DIR}" CACHE PATH "" FORCE)
set(PTHREAD_RELEASE_LIBRARY "${${proj}_INSTALL_DIR}/lib/libpthread.lib" CACHE FILEPATH "" FORCE)
set(PTHREAD_DEBUG_LIBRARY "${${proj}_INSTALL_DIR}/lib/libpthreadd.lib" CACHE FILEPATH "" FORCE)

if(NOT SB_SECOND_PASS)
  return()
endif()

set(${proj}_ROOT_DIR ${${proj}_INSTALL_DIR})
set(${proj}_LIB_DIR "lib")

ExternalProject_Message(${proj} "${proj}_ROOT_DIR:${${proj}_ROOT_DIR}")
ExternalProject_Message(${proj} "${proj}_LIB_DIR:${${proj}_LIB_DIR}")

ExternalProject_Message(${proj} "PTHREAD_INCLUDE_DIR:${PTHREAD_INCLUDE_DIR}")
ExternalProject_Message(${proj} "PTHREAD_RELEASE_LIBRARY:${PTHREAD_RELEASE_LIBRARY}")
ExternalProject_Message(${proj} "PTHREAD_DEBUG_LIBRARY:${PTHREAD_DEBUG_LIBRARY}")

mark_as_superbuild(
  VARS
    ${proj}_ROOT_DIR:PATH
    ${proj}_LIB_DIR:STRING
  PROJECTS
    iMSTK
  )

