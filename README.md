# Custom 3D Simulation & Rendering Engine

This is a **C++-based software rendering and simulation engine** built entirely from scratch. It includes everything from **triangle rasterization** and **3D projection algorithms** to basic utilities like **matrix operations** and **font rendering**, all implemented without relying on external libraries.

<p align="center">
    <img src="image.png" width="90%">
</p>

## Key Features:
- **Depth Buffering**: Solves the painter's algorithm problem for accurate depth rendering.
- **Triangle Rasterization**: Includes shading for improved visual quality.
- **Face Culling**: Skips rendering faces not visible to the camera to boost performance.
- **3D Object Loader**: Supports Wavefront (.obj) 3D models.
- **FPS Management**: Built-in FPS counter and limiter for smooth performance.
- **Pixel Buffer Masking**: Enables effects like removing water inside a boat.
- **Resizable Window**: Adjust window size dynamically without interruptions, handled by a dedicated thread.
- **Font Rendering**: Load and display custom fonts.
- **Buoyancy Simulation**: Dynamically calculates buoyant forces and torque to simulate realistic boat movement when hit by waves.

## How to Run:
1. Copy the executable folder to your computer.
2. Double-click `engine.exe` to start the application.
