#pragma once

#include <glm/glm.hpp>
#include <vector>

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

private:
    Robot robot;
    Target target;
    bool collision=false;
    bool success=false;
    bool checkCollision(glm::vec3 position) const;
    bool checkSuccess() const;
    std::vector<Obstacle> obstacles;
};