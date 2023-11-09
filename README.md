# 🌌 Custom-3D-Simulation & Rendering-Engine

This is a software rendering and simuilation engine developed purely in C++. Witness the intricate details as everything, from triangle rasterization to 3D projection algorithms, is crafted without leaning on any external libraries. Not just that, even foundational elements like matrix functions and font rendering are implemented from the ground up.

<p align="center">
    <img src="video.gif" width="90%" height="50%">
</p>

## 🎯 Current Features:
-  **Depth-buffering** See [painter's problem](https://en.wikipedia.org/wiki/Painter%27s_algorithm).
-  **Triangle Rasterization**: Enhanced with shading computations for richer visuals.
-  **Face Culling**: A face is not rendered if it "looks" aways from the camera to reduce render times and enhance performance.
-  **3D Object File loader**: supports wavefront 3D models.
-  **FPS Management**: An in-built FPS counter and limiter ensures the program runs smoothly.
-  **Pixel Buffer Masking**: For removing water inside a boat.
-  **Resizable Window**: Adjust the window size on-the-fly without any interruptions, an additional thread is utilized to to handle resizing of the windows.
-  **Font Management**: Load and render custom fonts.
-  **Bouyancy Estimation**: The bouyant forces under the boat is computed dynamically to compute the absolute torque and forces that orient and move the boat when wave hits.
  
