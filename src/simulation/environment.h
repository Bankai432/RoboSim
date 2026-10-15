#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "observation.h"

struct Robot{
    glm::vec3 position;
    float yaw;
};

struct Target{
    glm::vec3 position;
};

struct Obstacle{
    glm::vec3 position;
    glm::vec3 size;
};

enum class Action{
    FORWARD,
    LEFT,
    RIGHT,
    STOP
};
struct SensorConfig {
    std::size_t rayCount=5;
    float fieldOfView=3.14159265f;
    float maxRange=10.0f;
};

class Environment{
public:
    Environment();

    void reset();
    void step(Action action, float deltaTime);  

    bool isCollision() const;
    bool isSuccess() const;

    const Robot& getRobot() const;
    const Target& getTarget() const;
    const std::vector<Obstacle>& getObstacles() const;
    Observation observe() const;
    void setSensorConfig(const SensorConfig& config);

private:
    Robot robot;
    Target target;
    bool collision=false;
    bool success=false;
    bool checkCollision(glm::vec3 position) const;
    bool checkSuccess() const;
    std::vector<Obstacle> obstacles;
    float castRay(float relativeAngle, float maxRange) const;
    SensorConfig sensorConfig;
};