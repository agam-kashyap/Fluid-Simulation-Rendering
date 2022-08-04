# Introduction

This repository contains the code for performing [IISPH](https://cg.informatik.uni-freiburg.de/publications/2013_TVCG_IISPH.pdf) based Fluid Simulation and rendering the same using [Screen Space Fluid Rendering](http://www.cs.rug.nl/~roe/publications/fluidcurvature.pdf). Currently tested on a Linux based system, with 16GB RAM, 940MX graphics card without CUDA support. 

# Requirements

- cmake >3.0.0
- OpenGL v3.3.0
- glm
- GLFW

# Running Code

1. Open Project Source code with CMake application
2. Select the Modules to Build
3. Configure using `Unix Makefiles`
4. Generate Binaries
5. Run `make all` in the folder with binaries

# Outputs

![DAM break Demo](Resources/dam.gif)

![Spherical Water Droplet Falling](Resources/sphere.gif)
