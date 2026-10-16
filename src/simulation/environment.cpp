#include "Environment.h"
#include <cmath>
#include <algorithm>
#include <stdexcept>

Environment::Environment(){
    reset();
}

void Environment::reset(){
    collision=false;
    success=false;
    robot.position=glm::vec3(0.0f,0.5f,0.0f);
    robot.yaw=0.0f;

    target.position=glm::vec3(-5.7f,0.5f,4.0f);

    obstacles.clear();

    // Central obstacle
    obstacles.push_back({
        glm::vec3(2.0f, 0.5f, 2.0f),
        glm::vec3(1.5f, 1.0f, 1.5f)
    });

    // Upper-left area: staggered barriers
    obstacles.push_back({
        glm::vec3(-5.0f, 0.5f, -4.0f),
        glm::vec3(1.0f, 1.0f, 5.0f)
    });

    obstacles.push_back({
        glm::vec3(-2.0f, 0.5f, -7.0f),
        glm::vec3(5.0f, 1.0f, 1.0f)
    });

    // Upper-middle: partial corridor
    obstacles.push_back({
        glm::vec3(3.0f, 0.5f, -7.0f),
        glm::vec3(1.0f, 1.0f, 6.0f)
    });

    obstacles.push_back({
        glm::vec3(7.0f, 0.5f, -3.0f),
        glm::vec3(5.0f, 1.0f, 1.0f)
    });

    // Lower-left: offset passage
    obstacles.push_back({
        glm::vec3(-8.0f, 0.5f, 3.0f),
        glm::vec3(1.0f, 1.0f, 8.0f)
    });

    obstacles.push_back({
        glm::vec3(-5.0f, 0.5f, 7.0f),
        glm::vec3(5.0f, 1.0f, 1.0f)
    });

    // Lower-middle: isolated obstacle
    obstacles.push_back({
        glm::vec3(1.0f, 0.5f, 8.0f),
        glm::vec3(2.0f, 1.0f, 2.0f)
    });

    // Right side: staggered barriers
    obstacles.push_back({
        glm::vec3(10.0f, 0.5f, 5.0f),
        glm::vec3(1.0f, 1.0f, 7.0f)
    });

    obstacles.push_back({
        glm::vec3(6.0f, 0.5f, 10.0f),
        glm::vec3(7.0f, 1.0f, 1.0f)
    });
    obstacles.push_back({
        glm::vec3(0.0f, 0.5f, -20.0f),
        glm::vec3(40.0f, 1.0f, 0.5f)
    });
    obstacles.push_back({
        glm::vec3(0.0f, 0.5f, 20.0f),
        glm::vec3(40.0f, 1.0f, 0.5f)
    });
    obstacles.push_back({
        glm::vec3(-20.0f, 0.5f, 0.0f),
        glm::vec3(0.5f, 1.0f, 40.0f)
    });
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

    for(const auto& obstacle:obstacles){
        float minX=obstacle.position.x-obstacle.size.x*0.5f;
        float maxX=obstacle.position.x+obstacle.size.x*0.5f;

        float minZ=obstacle.position.z-obstacle.size.z*0.5f;
        float maxZ=obstacle.position.z+obstacle.size.z*0.5f;

        float closestX=std::max(minX,std::min(position.x,maxX));
        float closestZ=std::max(minZ,std::min(position.z,maxZ));

        float dx=position.x-closestX;
        float dz=position.z-closestZ;

        if(dx*dx+dz*dz<robotRadius*robotRadius) return true;
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

    const auto count = sensorConfig.rayCount;
    const float fov = sensorConfig.fieldOfView;

    obs.rayDistances.reserve(count);

    for(std::size_t i=0;i<count;i++){
        float angle=0.0f;
        if(count>1) angle=-fov*0.5f+fov*static_cast<float>(i)/static_cast<float>(count-1);
        obs.rayDistances.push_back(castRay(angle,sensorConfig.maxRange));
    }
    return obs;
}
void Environment::setSensorConfig(const SensorConfig& config) {
    if(config.rayCount<=0) throw std::invalid_argument("Ray count must be positive");
    if(config.fieldOfView<0.0f || config.fieldOfView>6.28318531f) throw std::invalid_argument("Invalid field of view");
    if(config.maxRange<=0.0f) throw std::invalid_argument("Max range must be positive");
    sensorConfig=config;
}
