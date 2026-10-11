# RoboSim

RoboSim is a lightweight robotics simulation prototype built with C++ and OpenGL. The project combines a simulation environment with a real-time 3D rendering pipeline, with planned support for sensor-driven navigation.

## Current Milestone

This milestone establishes the simulation foundation and a basic 3D rendering pipeline.

### Implemented Features

- Robot state with position and heading.
- Time-dependent movement using elapsed frame time.
- Forward movement and left/right turning.
- Obstacle collision detection and goal detection.
- Environment reset functionality.
- OpenGL context and shader initialization.
- Vertex buffer and vertex array configuration.
- 3D cube geometry with surface normals.
- Model, view, and perspective projection transformations.
- Depth-buffer-based visibility.
- Basic ambient and diffuse lighting.
- Terminal output for position, heading, collision state, and goal status.

The simulation and rendering systems are not yet fully integrated. The current renderer displays test geometry rather than a scene driven by live simulation state.

## Technology Stack

- **Language:** C++
- **Graphics API:** OpenGL
- **Windowing and input:** GLFW
- **OpenGL function loading:** GLAD
- **Linear algebra:** GLM
- **Shaders:** GLSL

## Architecture

### Simulation

The `Environment` component manages robot state and environment rules, including movement, collision checks, goal detection, and reset behavior. Movement updates use elapsed frame time.

### Rendering

The rendering layer initializes OpenGL resources, loads shader programs, configures vertex attributes, and submits geometry for drawing. Vertex positions and surface normals are passed to the vertex shader as attributes.

### Rendering Pipeline

1. Local-space vertex coordinates.
2. Model transformation into world space.
3. View transformation into camera space.
4. Perspective projection into clip space.
5. Clipping, perspective division, and viewport mapping.
6. Fragment processing and depth testing.
7. Ambient and diffuse lighting.

### Lighting

The current lighting model combines constant ambient illumination with Lambertian diffuse lighting. The diffuse contribution is calculated from the dot product of the normalized surface normal and light direction, clamped to non-negative values. Shadows and specular highlights are not implemented.

## Controls

| Key | Action |
| --- | --- |
| `W` | Move forward |
| `A` | Turn left |
| `D` | Turn right |
| `R` | Reset the environment |

## Build Requirements

The current development setup uses Windows, MSYS2 UCRT64, GCC, and VS Code.

Dependencies:
- A compatible C++ compiler
- GLFW
- GLAD
- GLM
- OpenGL support

Configure the compiler, include paths, and linker settings for the installed dependencies. Build the application using the configured VS Code build task. Run the executable from the project root so relative shader paths resolve correctly.

## Current Limitations

- The renderer is not connected to live environment object transforms.
- The arena, robot, obstacles, and target are not yet represented as an integrated rendered scene.
- Sensor observations and autonomous navigation are not implemented.
- Lighting is limited to ambient and diffuse contributions.
- The camera is fixed.

## Planned Work

1. Render the arena and environment objects from simulation state.
2. Synchronize the rendered robot with its simulated position and orientation.
3. Add sensor observations for navigation.
4. Implement a reactive autonomous controller.
5. Integrate and test the simulation and rendering systems.

## Project Status

**Milestone:** Simulation foundation and 3D rendering prototype.
