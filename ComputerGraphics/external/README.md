# External Libraries Setup

This directory contains third-party libraries required for 3D graphics functionality.

## Required Libraries

### 1. GLAD (OpenGL Loader)
- **Purpose**: Load OpenGL functions
- **Version**: OpenGL 3.3 Core Profile
- **Setup**:
  1. Visit https://glad.dav1d.de/
  2. Select: Language=C/C++, Specification=OpenGL, API gl=Version 3.3, Profile=Core
  3. Generate and download
  4. Extract `glad/` folder here: `external/glad/`
  5. Extract `KHR/` folder here: `external/KHR/`
  6. Place `glad.c` in `external/src/`

### 2. GLM (OpenGL Mathematics)
- **Purpose**: Math library for graphics
- **Version**: 0.9.9+
- **Setup**:
  1. Download from https://github.com/g-truc/glm/releases
  2. Extract `glm/` folder here: `external/glm/`
  3. Header-only library, no compilation needed

### 3. stb_image (Image Loader)
- **Purpose**: Load texture images
- **Version**: Latest
- **Setup**:
  1. Download `stb_image.h` from https://github.com/nothings/stb
  2. Place in `external/stb/`

## Directory Structure After Setup

```
external/
├── README.md (this file)
├── glad/
│   └── glad.h
├── KHR/
│   └── khrplatform.h
├── glm/
│   └── glm/
│       ├── glm.hpp
│       ├── gtc/
│       └── gtx/
├── stb/
│   └── stb_image.h
└── src/
    └── glad.c
```

## Integration Notes

- GLAD requires `glad.c` to be compiled with the project
- GLM is header-only, just include the headers
- stb_image requires `#define STB_IMAGE_IMPLEMENTATION` in one .cpp file before including
