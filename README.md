# Custom-3D-Software-Rendering-Engine
This is a rendering engine that I developed in C from scratch without using any library. I
I programmed everything in completely from sctrach including triangle rasterization and 3D projection. Not library is used. I even wrote the matrix calculation by hand.

![image](video.gif)

# Current Features:
* depth-buffering for the painters problem.
* triangle rasterization with antialiasing and shading calculations
* back face culling for optimizing rednering
* wavefront file format loading
* sphere rendering without geometry data(index and vertex buffer)
* triangle mesh index and vertex buffer generation
* FPS counter and limiter
* pixel buffer masking (for removing water inside the boat)
* resiable window size without blocking the program (I used seprate threads for handling client messages and for the main program)
* used multithreading for rendering triangles, and smiulating the environment dyanmics.
* implemented triangle clipping algotihm for optimizing the rasterization process.
* custom font loading and rendering
* fast triangle mesh intersection detection
* graph generation for mesh surface and objects (generates neighbor list of triangles)
