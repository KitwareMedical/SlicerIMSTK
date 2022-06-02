project(vtkSlicer${MODULE_NAME}ModuleLogic)

set(KIT ${PROJECT_NAME})

find_package(iMSTK 5.0 REQUIRED)

set(iMSTK_LIBRARIES
  imstk::Animation
  imstk::CollisionDetection
  imstk::CollisionHandling
  imstk::Controllers
  imstk::Filtering
  imstk::FilteringCore
  imstk::Geometry
  imstk::MeshIO
  imstk::RenderingCore
  imstk::Scene
  imstk::SceneEntities
  imstk::SimulationManager
  )
if(SlicerIMSTK_BUILD_ViewerVTK)
  list(APPEND iMSTK_LIBRARIES
    imstk::RenderingVTK
    imstk::ViewerVTK
    )
endif()

set(${KIT}_EXPORT_DIRECTIVE "VTK_SLICER_${MODULE_NAME_UPPER}_MODULE_LOGIC_EXPORT")

set(${KIT}_INCLUDE_DIRECTORIES
  )

set(${KIT}_SRCS
  vtkSlicer${MODULE_NAME}Logic.cxx
  vtkSlicer${MODULE_NAME}Logic.h
  )

set(${KIT}_TARGET_LIBRARIES
  ${ITK_LIBRARIES}
  ${iMSTK_LIBRARIES}
  )

set(Slicer_iMSTK_USE_OpenHaptics ${iMSTK_USE_OpenHaptics})
set(configure_header_file vtkSlicerIMSTKLogicConfigure.h)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/${configure_header_file}.in
  ${CMAKE_CURRENT_BINARY_DIR}/${configure_header_file}
  )

#-----------------------------------------------------------------------------
SlicerMacroBuildModuleLogic(
  NAME ${KIT}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${${KIT}_INCLUDE_DIRECTORIES}
  SRCS ${${KIT}_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  )
