# Custom-3D-Simulation & Rendering-Engine

A comprehensive simulation and rendering engine meticulously developed in C without relying on external libraries. This engine embodies precision engineering, with everything from triangle rasterization to 3D projection algorithms constructed in-house. Core functionalities such as matrix operations and font rendering have also been internally developed.

![Engine-Demo](video.gif)

## Features:

- **Depth-buffering**: Efficiently resolves the [painter's algorithm](https://en.wikipedia.org/wiki/Painter%27s_algorithm) issue.
- **Triangle Rasterization**: Enhanced with antialiasing and intricate shading calculations.
- **Back Face Culling**: Implemented to optimize rendering processes.
- **File Format Adaptability**: Supports the wavefront file format.
- **Sphere Rendering**: Capable of rendering spheres without the necessity of geometry data.
- **FPS Management**: Integrated FPS counter and limiter ensure stable performance.
- **Pixel Buffer Masking**: Advanced feature for selective rendering, such as omitting water within a vessel.
- **Dynamic Resizing**: Engine supports seamless window resizing due to multi-threaded architecture.
- **Triangle Clipping**: An algorithm designed to further optimize the rasterization process.
- **Font Integration**: Custom font loading and rendering capabilities.
- **Intersection Detection**: Efficient algorithms for rapid triangle mesh intersection identification.
- **Graphical Representations**: Enables the generation of graphs for mesh surfaces and objects.

## Installation:

1. **Repository Cloning**:
   - `git clone <repository-link>`
   - Refer to the accompanying documentation for platform-specific setup instructions.

2. **Execution**:
   - Initiate the program post-setup to engage with the simulation and rendering functionalities.
   - The engine supports the loading of custom models and offers a range of configurable settings.

## Future Enhancements:

- Integration of real-time shadows and reflections.
- Expansion in supported file formats.
- Incorporation of advanced spatial data structures for enhanced performance.
- Addition of sophisticated physical simulations and environmental effects.

## Contribution:

We encourage collaboration. Kindly submit pull requests for enhancements, suggest features, or report any discrepancies.

## Licensing:

This project is licensed under the terms of the MIT license.

---

Leverage the precision and efficiency of the Custom-3D-Simulation & Rendering-Engine for unparalleled rendering experiences.
