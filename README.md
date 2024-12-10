# GUI Toolkit for Wayland

## Members: 
- [Marta Samoilenko](https://github.com/MartaSamoilenkoPn)
- [Iryna Kokhan](https://github.com/ironiss)
- [Anna Yaremko](https://github.com/moisamidi)
- [Bohdan Pavliuk](https://github.com/BohdanPavliuk)

---
## Setup:
- Download the library from Cairo’s official site.
- Compile the library manually with OpenGL support,
ensuring that you configure the build process to include EGL
and GL capabilities.

  ```
  ./autogen.sh --enable-gl --enable-egl
  ```
- When linking Cairo to your project, you must explicitly
define the path to your custom-built library, as system-wide in-
stalled versions may not include the required OpenGL features
or may conflict with your build.

---
## Usage:
```
mkdir build && cd build && cmake .. && make
```

```
./GWayTool
```

You should import the header called `application.h` and then you will be able to use defined structures. Example of the usage is presented in the `examples/main.cpp`

The report you can find here:

https://www.overleaf.com/read/tdnhkxtmgspr#555a62
