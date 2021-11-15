SlicerIMSTK
===========

Extension for 3D slicer that enables user to prototype real-time multi-modal surgical simulation scenarios
by leveraging [iMSTK][iMSTK].

_This project is in active development and may change from version to version without notice,_

[iMSTK]: https://imstk.readthedocs.io

Use cases
---------

To enable the use of iMSTK in Slicer based application, the SlicerIMSTK extension
supports two use cases:

| Use case | Description |
|----------|-------------|
| Bundled | Bundling in Slicer custom application. This provides all dependencies, configures & builds iMSTK and defines packaging rules to support integration in custom application created using [KitwareMedical/SlicerCustomAppTemplate][SlicerCustomAppTemplate]. |
| Standalone | Building as a standalone extension. This provides iMSTK functionalities to the Slicer community through the extension manager. |

[SlicerCustomAppTemplate]: https://github.com/KitwareMedical/SlicerCustomAppTemplate


Frequently asked questions
--------------------------

### How to bundle the SlicerIMSTK extension in a Slicer custom application ?

A snippet like the following should be added in the custom application `CMakeLists.txt`.

Make sure to replace `<SHA>` with a valid value.

Note the inclusion of `SuperBuildPrerequisites.cmake` CMake module after the call to `FetchContent_Populate`.

```cmake
# Add remote extension source directories

# SlicerIMSTK
set(extension_name "SlicerIMSTK")
set(${extension_name}_SOURCE_DIR "${CMAKE_BINARY_DIR}/${extension_name}")
FetchContent_Populate(${extension_name}
  SOURCE_DIR     ${${extension_name}_SOURCE_DIR}
  GIT_REPOSITORY ${EP_GIT_PROTOCOL}://github.com/KitwareMedical/SlicerIMSTK.git
  GIT_TAG        <SHA>
  GIT_PROGRESS   1
  QUIET
  )
message(STATUS "Remote - ${extension_name} [OK]")
list(APPEND Slicer_EXTENSION_SOURCE_DIRS ${${extension_name}_SOURCE_DIR})

include(${SlicerIMSTK_SOURCE_DIR}/SuperBuildPrerequisites.cmake)
```

How to cite
-----------

`Arikatla, Venkata S., Mohit Tyagi, Andinet Enquobahrie, Tung Nguyen, George H. Blakey, Ray White, and Beatriz Paniagua. "High fidelity virtual reality orthognathic surgery simulator." In Medical Imaging 2018: Image-Guided Procedures, Robotic Interventions, and Modeling, vol. 10576, p. 1057612. International Society for Optics and Photonics, 2018., doi: 10.1117/12.2293690`

License
-------

It is covered by the Apache License, Version 2.0:

http://www.apache.org/licenses/LICENSE-2.0
