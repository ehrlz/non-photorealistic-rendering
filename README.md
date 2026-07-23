
<h1 align="center">
  <br>
  Non Photorealistic Rendering
  <br>
</h1>

<h4 align="center">Non-photorealistic rendering sandbox to experiment with diferents shaders and params.</h4>


<p align="center">
  <a>
    <img src="https://img.shields.io/badge/OpenGL-%23FFFFFF.svg?style=for-the-badge&logo=opengl">
  </a>
  <a>
    <img src="https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white">
  </a>
  <br>
   <a>
    <img src="https://img.shields.io/badge/License-MIT-yellow.svg">
  </a>
</p>

<p align="center">
  <a href="#how-to-use">Installation</a> •
  <a href="#shaders">Shaders</a>
</p>


https://github.com/ehrlz/non-photorealistic-rendering/assets/62675568/3aa8c7c5-6a1c-4a76-9835-411a9169cbbd


## Goal
This project focuses on the implementation and exploration of non-photorealistic rendering (NPR) techniques for generating computer graphics.
Unlike photorealistic rendering, which aims to replicate reality as closely as possible, NPR emphasizes more artistic and stylized visual styles.
The goal is to provide a tool to experiment with a collection of npr algorithms and techniques that allow developers and graphic artists to create images and scenes with unique and stylized appearances.


## Features
* Different shaders and models to experiment
* Postrender effects
* Customizable shaders 
  - Many params as light direction or pixelating size can be selected
* Texture render switch
  - Color selector
* .obj loader
  - Custom .obj parser with no external libraries
* Cross platform
  - Windows and Linux ready.


## How to use

### Requirements

- CMake >= 3.20
- Git
- OpenGL libs: `sudo dnf install mesa-libGL-devel mesa-libGLU-devel`
- X11 develop. tools: `sudo dnf install libXrandr-devel libXinerama-devel libXcursor-devel libXi-devel`
- Wayland develop. tools: `sudo dnf install wayland-devel libxkbcommon-devel`

> (Fedora44 packages)

> [!WARNING]
> Only X11 or Wayland is requested. Turn off the option you prefer in CMakeFiles.txt. e.g. `set(GLFW_BUILD_WAYLAND OFF...)` 

```bash
# Clone this repo
git clone git@github.com:ehrlz/non-photorealistic-rendering.git
```

```
# Go into the repo
cd non-photorealistic-rendering
```
```
# Build it
cmake -B build -DCMAKE_POLICY_VERSION_MINIMUM=3.5 && cmake --build build
```

```
# Run the app (in `/build` folder)
./bin/gpo_proyecto_final
```
> [!TIP]
> Ubuntu 20.04 CMake 3.2 version: `sudo snap install cmake --classic`

## Shaders
### Phong

Basic local ilumination shader.

![cat](https://github.com/ehrlz/non-photorealistic-rendering/assets/62675568/e8ad6cdd-c239-4d88-8516-f7847ab6ddc9)

### Blinn-Phong

Modification to the previous shader. Obtains an aproximation with less calculations.

### Toon shading (cell shading)

Combination of an ilumination shader (blinn in this case), color discretization and silhoutte rendering.

![toon](https://github.com/ehrlz/non-photorealistic-rendering/assets/62675568/894d2111-7862-45d2-88c0-e8548cabb847)

### Gooch shading

2 colors in conjunction with the original model color. Warm color (such as yellow) and cool color (blue). The warm color indicates that are facing toward the light source while the cool color indicates surfaces facing away.

![gooch](https://github.com/ehrlz/non-photorealistic-rendering/assets/62675568/7a4402c8-102d-46a0-9897-bece1dad2704)

### Pixel shading

Postprocessing effect that discretizates the render in pixels with customizable size.

![pixel](https://github.com/ehrlz/non-photorealistic-rendering/assets/62675568/36238866-30f0-4977-b2b9-83cad02e9be0)

### Pixel texture rendering

Shader that pixelates the texture that is applied to a model.

![texture_pixel](https://github.com/ehrlz/non-photorealistic-rendering/assets/62675568/75c50ff9-0580-4279-b59e-ecf8b5b4d2fb)

## Credits
This software uses [imgui](https://github.com/ocornut/imgui) to handle user interactions with the scenes params.
