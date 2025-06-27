<h1 align="center">🎯 Ray Tracer</h1>

<p align="center">
  A small educational ray tracer built from scratch, inspired by the book:<br>
  <a href="https://raytracing.github.io/books/RayTracingInOneWeekend.html"><em>Ray Tracing in One Weekend</em></a>
</p>

---

## 🌟 About

This project is a personal exploration of ray tracing fundamentals.  
It's built for learning purposes, so things might be imperfect — and that's totally fine.

The code follows along with Peter Shirley's guide, while adding some modern twists like:
- Real-time preview using ImGui
- Cross-platform support (Windows & Linux)
- Modular C++17 codebase

---

## 📦 Installation

### 🔧 Prerequisites

Make sure the following tools/libraries are installed:

- C++17-compatible compiler (`g++`, `clang++`, or MSVC)
- CMake ≥ 3.10
- Git
- OpenGL development libraries
    - **Linux**: `libgl1-mesa-dev`, `xorg-dev`, etc.
    - **Windows**: Comes with most toolchains or via vcpkg

---

### 📥 Clone the Repository

```bash
git clone --recurse-submodules https://github.com/DjCtavia/raytracer.git
cd raytracer
```
⚠️ If you forgot --recurse-submodules:
```bash
git submodule update --init --recursive
```

---

### Install GLAD (OpenGL Loader)

GLAD is **not included as a submodule**, so you need to generate and install it manually:

1. Go to [https://glad.dav1d.de/](https://glad.dav1d.de/)
2. Configure the generator:
    - **Language**: C/C++
    - **Specification**: OpenGL
    - **API**: gl (version 3.3+), Core profile
    - ✅ Check "Generate a loader"
3. Click **Generate** and download the ZIP
4. Extract it and copy the contents into the `vendors/glad/` directory

---

### ⚙️ Build the Project
```bash
mkdir build
cd build
cmake ..
make
```