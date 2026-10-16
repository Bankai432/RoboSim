# RoboSim Lite

RoboSim Lite is a lightweight robotics simulation prototype built with C++ and OpenGL. It combines a deterministic time-stepped simulation environment, a real-time 3D renderer, configurable range sensing, and a reactive agent interface for experimenting with sensor-driven autonomous navigation.

## Demo

[Watch the RoboSim demo](demo/RoboSim_DEMO.mp4)

## Implemented Features

### Simulation
- Robot pose (XZ position + heading) updated directly by discrete actions: forward, turn left, turn right, stop. There is no wheel or dynamics model.
- Fixed-timestep updates via an accumulator, independent of frame rate.
- Circle-vs-AABB collision in the XZ plane. A proposed move is checked before it is committed.
- Target detection, success state, reset, perimeter walls.
- All of this lives in `Environment`, advanced by `Environment::step(action, dt)`.

### Sensing and Agent Interface

- Configurable ray-based range sensing.
- Current configuration: 16 rays spanning a 180° field of view and maximum sensing range of 8 world units.
- Ray–axis-aligned bounding box intersection tests.
- Observation structure containing measured ray distances.
- Sensor measurements are shared between the simulation interface and renderer.

The current observation provides range measurements only. Target-relative information is not exposed to the agent.

### Rendering

- OpenGL/GLSL, VAO/VBO, reusable cube mesh with normals, MVP transforms, depth testing, follow camera.
- Ambient + Lambertian diffuse lighting, per-object colour via uniforms.
- Robot colour changes on collision and success.
- Sensor rays are drawn from live measurements, with a distinct colour for hits vs max-range readings.


### Lighting and Visual Feedback

- Basic ambient and Lambertian diffuse lighting.
- Configurable object colors through shader uniforms.
- Robot visual state changes based on collision and success.
- Distinct visual representation for target and obstacles.
- Sensor rays distinguish detected obstacles from maximum-range measurements.

## Architecture

### Simulation Environment

The `Environment` component owns the robot state, target, obstacle data, movement, collision logic, success detection, sensing, and reset behavior.

Simulation transitions occur through:

```cpp
Environment::step(action, dt)
```

The current agent is a rule-based reactive baseline rather than a learned policy or complete navigation algorithm.

The agent implementation is present, but the current graphical application can use keyboard input as the active controller. Autonomous agent control can be enabled in the control path without changing the underlying simulation architecture.

This separation allows the controller to eventually be replaced by a search-based or learned policy without redesigning the environment.

## Reliability Checks

The current implementation has been manually checked for:

- **Collision behavior:** Movement into obstacles and perimeter walls is rejected by the simulation collision checks.
- **Goal detection:** Entering the target detection region sets the success state.
- **Reset behavior:** Reset restores the initial environment state.
- **Sensor consistency:** Repeated observations in an unchanged scenario produce identical ray-distance measurements.
- **Simulation repeatability:** Repeated runs with identical initial conditions and action sequences produce consistent simulation results.

These checks support deterministic behavior under the tested conditions. They are not an exhaustive automated test suite or a formal guarantee across platforms.

## Controls

| Key | Action |
|---|---|
| `W` | Move forward |
| `A` | Turn left |
| `D` | Turn right |
| `R` | Reset |

Keyboard control can be used when manual control is enabled. The reactive agent is implemented separately from the input mechanism. 

## Technology Stack

- **Language:** C++17
- **Graphics API:** OpenGL
- **Windowing and input:** GLFW
- **OpenGL function loading:** GLAD
- **Linear algebra:** GLM
- **Shaders:** GLSL
- **Build environment:** Windows, MSYS2 UCRT64, GCC, and VS Code



## Current Limitations

- **Agent is blind to the target.** Observations contain no target direction or distance, so the agent avoids obstacles but cannot navigate toward the goal. Any success is incidental.
- **No memory or map.** The agent is purely reactive and can get stuck in some layouts.
- **Collision footprint** is a circle that doesn't exactly match the rendered robot. Collision checks are discrete, so large steps could tunnel through thin geometry.
- **Controller switch is compile-time.** Changing it means editing the loop and rebuilding.
- **No automated tests.** The checks above were done by hand, and repeatability is verified only on the tested setup.
- **Lighting** is ambient + diffuse only: no shadows or specular.

## Planned Work

1. Enable autonomous agent control as a selectable controller in the graphical application.
2. Separate simulation execution from the graphical application to support headless evaluation.
3. Formalize episode termination and reset semantics.
4. Add automated tests for deterministic stepping, collision invariants, reset behavior, and sensor consistency.
5. Evaluate navigation policies across different obstacle layouts and target positions.
6. Extend the observation and action interfaces for future search-based or learned controllers.
7. Improve collision handling where required by the simulator's core objectives.

## Project Status

**Current milestone:** Integrated simulation, fixed-step updates, configurable range sensing, reactive agent interface, and sensor visualization.

**Next milestone:** Autonomous control selection, headless evaluation, and systematic navigation testing.