# RoboSim Lite

RoboSim Lite is a lightweight robotics simulation prototype built with C++ and OpenGL. It combines a time-stepped simulation environment, a real-time 3D renderer, and a restricted sensor interface for experimenting with sensor-driven autonomous navigation.

## Current Milestone

**Deterministic simulation, sensor integration, and reactive control**

The environment supports robot movement, obstacle collisions, target detection, reset functionality, and simulated range sensing. A reactive agent consumes observations and selects actions through a defined interface. The renderer visualizes the environment and sensor readings.

### Implemented Features

**Simulation**
- Robot state represented by position and heading.
- Fixed-step simulation updates using a configurable timestep.
- Forward movement and left/right rotation.
- Obstacle collision detection using circle-versus-AABB intersection in the XZ plane.
- Goal detection and environment reset functionality.
- Collision and success state tracking.
- Enclosed environment with perimeter walls.

**Sensing and Agent Interface**
- Five-ray range sensor with relative angles of −90°, −45°, 0°, +45°, and +90°.
- Ray–axis-aligned bounding box intersection tests to estimate obstacle distances.
- Configurable maximum sensing range.
- Observation structure containing the five ray distances.
- Reactive agent that maps observations to discrete actions.
- Agent decisions are separated from direct access to the environment's internal state.

**Rendering**
- OpenGL context and GLSL shader initialization.
- VAO/VBO configuration and reusable 3D cube geometry with surface normals.
- Model, view, and perspective projection transformations.
- Depth-buffer-based visibility.
- Ground plane, robot, target, obstacles, and perimeter walls rendered from simulation state.
- Smoothed camera following the robot.
- Framebuffer resizing and viewport updates.
- Dynamic visualization of the five sensor rays, using measured sensor distances.

**Lighting and Visual Feedback**
- Basic ambient and Lambertian diffuse lighting.
- Configurable object colors through shader uniforms.
- Robot color changes based on collision and success state.
- Distinct visual colors for the target and obstacles.
- Sensor-ray colors distinguish obstacle detections from maximum-range readings.

## Technology Stack

- **Language:** C++
- **Graphics API:** OpenGL
- **Windowing:** GLFW
- **OpenGL function loading:** GLAD
- **Linear algebra:** GLM
- **Shaders:** GLSL
- **Build environment:** MSYS2 UCRT64, GCC, and VS Code

## Architecture

### Simulation Environment

The `Environment` component owns the robot state, target, obstacle data, collision logic, success detection, sensing, and reset behavior.

Simulation transitions occur through `Environment::step(action, dt)`. A fixed-step accumulator in the application loop separates simulation update frequency from rendering frequency, reducing dependence on variable frame times.

Collision detection operates in the XZ plane. The robot is approximated by a circle and obstacles by axis-aligned bounding boxes. Proposed movement is checked before the new robot state is committed.

### Observation and Sensing

The environment exposes an `Observation` containing five ray distances. Each ray is defined relative to the robot's current heading and returns the distance to the nearest intersected obstacle, capped at the configured maximum range.

The observation interface deliberately exposes less information than the environment internally maintains. The agent receives range measurements rather than direct access to the robot's coordinates, target coordinates, or obstacle list.

The same observations drive the sensor-ray visualization, allowing the displayed rays to reflect the distances available to the agent.

### Agent and Action Loop

The control loop follows this sequence:

1. The environment produces an observation.
2. The agent selects an action from the observation.
3. The environment applies the action for one simulation timestep.
4. Collision and success conditions are updated.
5. The renderer displays the resulting state and sensor readings.

The current agent is a simple reactive baseline, not a learned policy or a complete navigation algorithm. Its purpose is to exercise the observation-to-action interface and provide a starting point for future controllers.

### Rendering Pipeline

1. Vertex positions and normals are supplied through vertex buffers.
2. Model transformations place and scale objects in world space.
3. The view transformation converts world coordinates into camera space.
4. Perspective projection transforms geometry into clip space.
5. Clipping, perspective division, and viewport mapping determine screen-space geometry.
6. Rasterization generates fragments, with depth testing resolving visibility.
7. Fragment shading applies ambient and diffuse lighting and the object's configured color.

The renderer reuses cube geometry for the robot, target, and obstacles. Sensor rays use a separate dynamic vertex buffer so their endpoints can be updated as the robot moves and turns.

### Lighting

The lighting model combines constant ambient illumination with Lambertian diffuse lighting. The diffuse contribution depends on the dot product between the normalized surface normal and the light direction, clamped to non-negative values.

Shadows, specular highlights, and physically based materials are not currently implemented.

## Reliability Checks

The current implementation has undergone the following manual checks:

- **Repeated-run consistency:** Three 30-second autonomous runs produced identical final robot X/Z positions and yaw.
- **Reset consistency:** Resetting the environment and repeating a 30-second run reproduced the same final values.
- **Collision behavior:** Manual tests against obstacles and walls showed no improper simulation penetration. Slight visual overlap can occur because the rendered geometry and simplified collision footprint do not match exactly.
- **Sensor consistency:** Repeated observations in the same scenario produced identical ray-distance readings.

These results support deterministic behavior under the tested conditions. They do not constitute exhaustive automated testing or a formal guarantee across platforms and execution conditions.

## Controls

| Key | Action |
|---|---|
| `W` | Move forward manually |
| `A` | Turn left manually |
| `D` | Turn right manually |
| `R` | Reset the environment |

The current main loop uses the autonomous agent to select simulation actions. Manual movement keys are therefore not currently the active controller unless manual control is re-enabled in the loop.

## Build Requirements

The current development setup uses Windows, MSYS2 UCRT64, GCC, and VS Code.

Dependencies:
- A compatible C++ compiler
- GLFW
- GLAD
- GLM
- OpenGL support

Configure the compiler, include paths, and linker settings for the installed dependencies. Build the application using the configured VS Code build task. Run the executable from the project root so relative shader paths resolve correctly.

Ensure that all simulation source files, including `Agent.cpp`, are included in the build configuration.

## Current Limitations

- The agent uses a simple reactive policy and is not expected to solve every navigation scenario.
- The agent has no persistent memory or internal map and does not receive target-relative information.
- The five-ray sensor provides sparse range measurements and can miss obstacles between ray directions.
- The robot's circular collision footprint does not exactly match its rendered geometry.
- Collision handling uses discrete movement checks and may require further work for high-speed motion.
- The current application loop integrates simulation and rendering; a standalone headless evaluation runner has not yet been established.
- Reliability checks are currently manual rather than part of an automated test suite.
- The lighting model is limited to ambient and diffuse contributions.

## Planned Work

1. Separate simulation execution from the graphical application to support headless evaluation.
2. Formalize episode termination and reset semantics.
3. Add automated tests for deterministic stepping, collision invariants, reset behavior, and sensor consistency.
4. Evaluate navigation policies across different obstacle layouts and target positions.
5. Extend the observation and control interfaces as needed for future search-based or learned agents.
6. Improve collision geometry and rendering where they support the simulator's core objective.

## Project Status

**Current milestone:** Integrated simulation, fixed-step updates, range sensing, reactive control, and sensor visualization.

**Next milestone:** Headless evaluation and systematic testing of the environment interface and navigation policies.