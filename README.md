# non-photorealistic-rendering


![OpenGL](https://img.shields.io/badge/OpenGL-%23FFFFFF.svg?style=for-the-badge&logo=opengl)
![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## Contenido
1. [Installation](#Instalación)
1. [Shaders](#Shaders)


## How to use

To run this application you'll need [Git](https://git-scm.com) and CMake (>=3.20):
```bash
# Clone this repo
git clone --recurse-submodules git@github.com:ehrlz/non-photorealistic-rendering.git

# Go into the repo
cd non-photorealistic-rendering/build

# Compile the project
cmake .. -DOpenGL_GL_PREFERENCE=GLVND
make -j4

# Run the app
./bin/gpo_proyecto_final
```
> [!TIP]
> Ubuntu 20.04 CMake 3.2 version: `sudo snap install cmake --classic`

> [!WARNING]
> `--recurse-submodules` flag is needed for `imgui` external 


## Shaders
### Phong

Basic local ilumination shader.

### Blinn-Phong

Modification to the previous shader. Obtains an aproximation with less calculations.

### Toon shading (cell shading)

Combination of an ilumination shader (blinn in this case), color discretization and silhoutte rendering.

### Gooch shading

2 colors in conjunction with the original model color. Warm color (such as yellow) and cool color (blue). The warm color indicates that are facing toward the light source while the cool color indicates surfaces facing away.

### Pixel shading

Postprocessing effect that discretizates the render in pixels with customizable size.

## Credits
This software uses [imgui](https://github.com/ocornut/imgui) to handle user interactions with the scenes params.