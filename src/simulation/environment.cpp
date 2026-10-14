#include "Environment.h"
#include <cmath>
#include <algorithm>

Environment::Environment(){
    reset();
}

void Environment::reset(){
    collision=false;
    success=false;
    robot.position=glm::vec3(0.0f,0.5f,0.0f);
    robot.yaw=0.0f;

    target.position=glm::vec3(3.0f,0.5f,3.0f);

    obstacles.clear();

    obstacles.push_back({
        glm::vec3(2.0f,0.5f,2.0f),
        glm::vec3(1.0f,1.0f,1.0f)
    });

    obstacles.push_back({
        glm::vec3(-5.0f, 0.5f,-4.0f),
        glm::vec3(1.0f,1.0f,2.0f)
    });
    obstacles.push_back({
        glm::vec3(0.0f,0.5f,-3.85f),
        glm::vec3(1.0f,1.0f,1.0f)
    });
    // North wall (-Z)
    obstacles.push_back({
        glm::vec3(0.0f, 0.5f, -20.0f),
        glm::vec3(40.0f, 1.0f, 0.5f)
    });

    // South wall (+Z)
    obstacles.push_back({
        glm::vec3(0.0f, 0.5f, 20.0f),
        glm::vec3(40.0f, 1.0f, 0.5f)
    });

    // West wall (-X)
    obstacles.push_back({
        glm::vec3(-20.0f, 0.5f, 0.0f),
        glm::vec3(0.5f, 1.0f, 40.0f)
    });

    // East wall (+X)
    obstacles.push_back({
        glm::vec3(20.0f, 0.5f, 0.0f),
        glm::vec3(0.5f, 1.0f, 40.0f)
    });
}
void Environment::step(Action action,float deltaTime){
    const float moveSpeed=3.0f;
    const float turnSpeed=2.0f;
    Robot next=robot;
    switch(action){
        case Action::FORWARD:{
            next.position.x+=std::sin(next.yaw)*moveSpeed*deltaTime;
            next.position.z-=std::cos(next.yaw)*moveSpeed*deltaTime;
            break;
        }
        case Action::LEFT:{
            next.yaw-=turnSpeed*deltaTime;
            break;
        }

        case Action::RIGHT:{
            next.yaw+=turnSpeed*deltaTime;
            break;
        }

        case Action::STOP: break;
    }
    if(checkCollision(next.position)){
        collision=true;
        return;
    }
    robot=next;
    success=checkSuccess();
}

bool Environment::checkCollision(glm::vec3 position) const{
    const float robotRadius=0.5f;

    for (const auto& obstacle:obstacles){
        float minX=obstacle.position.x-obstacle.size.x*0.5f;
        float maxX=obstacle.position.x+obstacle.size.x*0.5f;

        float minZ=obstacle.position.z-obstacle.size.z*0.5f;
        float maxZ=obstacle.position.z+obstacle.size.z*0.5f;

        float closestX=std::max(minX,std::min(position.x,maxX));
        float closestZ=std::max(minZ,std::min(position.z,maxZ));

        float dx=position.x-closestX;
        float dz=position.z-closestZ;

        if(dx*dx+dz*dz<robotRadius*robotRadius){
            return true;
        }
    }
    return false;
}

bool Environment::checkSuccess() const {
    float dx=robot.position.x-target.position.x;
    float dz=robot.position.z-target.position.z;
    const float goalRadius=0.5f;
    return dx*dx+dz*dz<goalRadius*goalRadius;
}

bool Environment::isCollision() const{
    return collision;
}

bool Environment::isSuccess() const{
    return success;
}

const Robot& Environment::getRobot() const{
    return robot;
}

const Target& Environment::getTarget() const{
    return target;
}

const std::vector<Obstacle>& Environment::getObstacles() const{
    return obstacles;
}


float Environment::castRay(float relativeAngle, float maxRange) const{

    const float rayYaw=robot.yaw+relativeAngle;

    const float dirX=sin(rayYaw);
    const float dirZ=-cos(rayYaw);

    float nearest=maxRange;
    for(auto const & obstacle:obstacles){
        const float minX=obstacle.position.x-obstacle.size.x*0.5f;
        const float maxX=obstacle.position.x+obstacle.size.x*0.5f;
        const float minZ=obstacle.position.z-obstacle.size.z*0.5f;
        const float maxZ=obstacle.position.z+obstacle.size.z*0.5f;        
        float tMin=0.0f;
        float tMax=maxRange;
        bool intersects=true;
        
        if(std::abs(dirX)<1e-6f && (robot.position.x<minX || robot.position.x>maxX)) intersects=false;
        else{
            float t1=(minX-robot.position.x)/dirX;
            float t2=(maxX-robot.position.x)/dirX;

            if(t1>t2) std::swap(t1,t2);
            tMin=std::max(tMin,t1);
            tMax=std::min(tMax,t2);

            if(tMin>tMax) intersects=false;
        }

        if(!intersects)continue;

        if(std::abs(dirZ)<1e-6f && (robot.position.z<minZ || robot.position.z>maxZ)) intersects=false;
        else{
            float t1=(minZ-robot.position.z)/dirZ;
            float t2=(maxZ-robot.position.z)/dirZ;

            if (t1>t2) std::swap(t1,t2);
            tMin=std::max(tMin,t1);
            tMax=std::min(tMax,t2);

            if(tMin>tMax) intersects=false;
        }
        if(intersects && tMax>=0.0f && tMin<=maxRange) nearest=std::min(nearest,tMin);
    }
    return nearest;
}

Observation Environment::observe() const{
    Observation obs{};

    constexpr float MAX_RANGE=5.0f;

    const std::array<float, 5> angles={
        -1.5707963f,  // -90 degrees
        -0.7853982f,  // -45 degrees
         0.0f,        // forward
         0.7853982f,  // +45 degrees
         1.5707963f   // +90 degrees
    };

    for(std::size_t i=0;i<angles.size();i++) {
        obs.rayDistances[i]=castRay(angles[i],MAX_RANGE);
    }

    return obs;
}
