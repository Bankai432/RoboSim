# RoboSim Lite

RoboSim Lite is a lightweight robotics simulation prototype built with C++ and OpenGL. It combines a time-stepped simulation environment, a real-time 3D renderer, configurable range sensing, and a reactive agent interface for experimenting with sensor-driven navigation.

The project is designed around a separation between simulation state, agent observations, discrete actions, and visualization.

## Current Milestone

**Integrated simulation, fixed-step updates, range sensing, and sensor visualization.**

The environment supports robot movement, obstacle collision detection, target detection, reset functionality, and configurable ray-based sensing. A reactive agent can select actions from sensor observations, while the OpenGL renderer visualizes the environment and measured sensor distances.

The simulation and agent interfaces are implemented, but the graphical application currently uses keyboard input to control the robot. Autonomous agent control is not yet connected to the active simulation loop.

## Implemented Features

### Simulation

- Robot state represented by position and heading.
- Fixed-step simulation updates using a timestep of 0.01 seconds.
- Discrete actions for forward movement, left rotation, right rotation, and stopping.
- Collision detection using circle-versus-axis-aligned bounding box intersection in the XZ plane.
- Goal detection and environment reset functionality.
- Collision and success state tracking.
- Enclosed environment with perimeter walls.
- Candidate-state validation before committing movement, preventing invalid movement through detected obstacles.

### Sensing and Agent Interface

- Configurable ray-based range sensing.
- Current configuration: 16 rays spanning a 180-degree field of view, with a maximum range of 8 world units.
- Ray–axis-aligned bounding box intersection tests to estimate the distance to the nearest obstacle.
- Observation structure containing ray-distance measurements.
- Reactive agent interface that maps observations to discrete actions.
- Separation between agent observations and the environment's internal state.
- Sensor measurements shared between the simulation interface and the renderer.

The agent receives range measurements rather than direct access to the environment's complete obstacle list or world state. The current observation does not provide target-relative information.

### Rendering

- OpenGL context and GLSL shader initialization.
- Vertex array and vertex buffer configuration.
- Reusable cube geometry with surface normals.
- Model, view, and perspective projection transformations.
- Depth-buffer-based visibility.
- Ground grid and environment objects rendered from simulation state.
- Robot, target, obstacles, and perimeter walls.
- Camera following the robot.
- Framebuffer resizing and viewport updates.
- Dynamic sensor-ray geometry updated using measured distances.

### Lighting and Visual Feedback

- Basic ambient and Lambertian diffuse lighting.
- Configurable object colors through shader uniforms.
- Robot color changes based on collision and success state.
- Distinct colors for the target and obstacles.
- Sensor-ray colors distinguish detected obstacles from rays that reach their maximum sensing range.

## Technology Stack

- **Language:** C++17
- **Graphics API:** OpenGL
- **Windowing:** GLFW
- **OpenGL function loading:** GLAD
- **Linear algebra:** GLM
- **Shaders:** GLSL
- **Build environment:** Windows, MSYS2 UCRT64, GCC, and VS Code

## Architecture

### Simulation Environment

The `Environment` component owns the robot state, target, obstacle data, collision logic, success detection, sensing, and reset behavior.

Simulation transitions occur through:

`Environment::step(action, dt)`

The graphical application uses a fixed-step accumulator to advance the simulation independently of the rendering frame rate. This reduces dependence on variable frame times.

Collision detection operates in the XZ plane. The robot is approximated by a circle and obstacles by axis-aligned bounding boxes. Proposed movement is checked before the updated robot state is committed.

### Observation and Sensing

The environment exposes an `Observation` containing ray-distance measurements. Rays are defined relative to the robot's heading and return the distance to the nearest intersected obstacle, capped at the configured maximum range.

The sensor configuration controls the number of rays, field of view, and maximum sensing distance. The current graphical application uses 16 rays over a 180-degree field of view, with a maximum range of 8 world units.

The observation interface deliberately exposes less information than the environment internally maintains. This provides a foundation for evaluating controllers that operate on limited sensor information instead of direct access to the complete simulation state.

The renderer uses the same sensor measurements to update the displayed ray endpoints.

### Agent and Control Loop

The intended autonomous control loop is:

1. The environment produces an observation.
2. The agent selects a discrete action from that observation.
3. The environment applies the action for one simulation timestep.
4. Collision and success conditions are updated.
5. The renderer displays the resulting state and sensor readings.

The reactive agent interface is implemented, but the autonomous action application is not currently active in the main graphical loop. Keyboard input remains the active controller.

The agent is a rule-based baseline, not a learned policy or a complete navigation algorithm. Connecting the agent to the simulation loop and evaluating its behavior across different environments remain future tasks.

### Rendering Pipeline

1. Vertex positions and normals are supplied through vertex buffers.
2. Model transformations place and scale objects in world space.
3. The view transformation converts world coordinates into camera space.
4. Perspective projection transforms geometry into clip space.
5. Clipping, perspective division, and viewport mapping determine screen-space geometry.
6. Rasterization generates fragments, with depth testing resolving visibility.
7. Fragment shading applies ambient and diffuse lighting and the object's configured color.

The renderer reuses cube geometry for environment objects. Sensor rays use a separate dynamic vertex buffer so their endpoints can be updated as the robot moves and turns.

### Lighting

The lighting model combines constant ambient illumination with Lambertian diffuse lighting. The diffuse contribution depends on the dot product between the normalized surface normal and the light direction, clamped to non-negative values.

Shadows, specular highlights, and physically based materials are not currently implemented.

## Reliability Checks

The implementation has been manually tested for the following behaviors:

- **Collision behavior:** Movement into obstacles is rejected by the simulation collision checks.
- **Goal detection:** Entering the target's detection radius sets the success state.
- **Reset behavior:** Reset restores the default robot state and environment configuration.
- **Sensor consistency:** Repeated observations in an unchanged scenario produce identical ray-distance measurements.
- **Simulation repeatability:** Repeated runs under identical initial conditions and action sequences produce consistent simulation results.

These checks support deterministic behavior for the tested scenarios. They do not constitute exhaustive automated testing or a formal guarantee across platforms and execution conditions. Rendering differences are also possible independently of simulation determinism.

## Controls

| Key | Action |
|---|---|
| `W` | Move forward |
| `A` | Turn left |
| `D` | Turn right |
| `R` | Reset the environment |

Keyboard input currently controls the simulation. The reactive agent is not yet the active controller in the graphical application.

## Build Requirements

The current development setup uses Windows, MSYS2 UCRT64, GCC, and VS Code.

Dependencies:

- A compatible C++17 compiler
- GLFW
- GLAD
- GLM
- OpenGL support

Configure the compiler, include paths, and linker settings for the installed dependencies. Build the application using the configured VS Code build task.

Run the executable from the project root so relative shader paths resolve correctly. Ensure that all required simulation and rendering source files, including `Agent.cpp`, are included in the build configuration.

## Current Limitations

- The reactive agent is not yet connected to the active graphical control loop.
- The baseline agent is rule-based and is not expected to solve every navigation scenario.
- The agent has no persistent memory or internal map.
- The observation contains sparse range measurements and no target-relative information.
- Obstacles between ray directions may be missed by the sensor.
- The robot's circular collision footprint does not exactly match its rendered geometry.
- Discrete collision checks may require further work for high-speed motion and more complex collision scenarios.
- Simulation and rendering are integrated in the graphical application; a standalone headless evaluation runner has not yet been established.
- Reliability checks are manual rather than part of an automated test suite.
- The lighting model is limited to ambient and diffuse contributions.

## Planned Work

1. Connect the reactive agent to the simulation loop and make autonomous control selectable independently of manual input.
2. Separate simulation execution from the graphical application to support headless evaluation.
3. Formalize episode termination and reset semantics.
4. Add automated tests for deterministic stepping, collision invariants, reset behavior, and sensor consistency.
5. Evaluate navigation policies across different obstacle layouts and target positions.
6. Extend the observation and control interfaces as needed for search-based or learned agents.
7. Improve collision handling and rendering where they support the simulator's core objectives.

## Project Status

**Current milestone:** Integrated simulation, fixed-step updates, configurable range sensing, reactive agent interface, and sensor visualization.

**Next milestone:** Connect autonomous control, establish headless evaluation, and systematically test the environment and navigation policies.