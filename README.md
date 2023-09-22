# Custom-3D-Simulation-&-Rendering-Engine
This is a simulation and rendering engine that is developed in C from scratch without using any library.
Everything is programmed completely from sctrach including triangle rasterization and 3D projection algorithms. No library is used. Even the matrix functions are implemented from scratch.

![image](video.gif)

#algorithms implemented epth buffering [painters problem](https://en.wikipedia.org/wiki/Painter%27s_algorithm)

# Current Features:
* depth-buffering
* triangle rasterization with antialiasing and shading calculations
* back face culling for optimizing rednering
* wavefront file format loading
* sphere rendering without geometry data(index and vertex buffer)
* triangle mesh index and vertex buffer generation
* FPS counter and limiter
* pixel buffer masking (for removing water inside the boat)
* resiable window size without blocking the program (I used seprate threads for handling client messages and for the main program)
* implemented triangle clipping algotihm for optimizing the rasterization process.
* custom font loading and rendering
* fast triangle mesh intersection detection
* graph generation for mesh surface and objects (generates neighbor list of triangles)
