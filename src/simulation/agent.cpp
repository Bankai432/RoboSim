#include "Agent.h"

Action Agent::chooseAct(const Observation& obs) const{


    constexpr float SAFE_DISTANCE=3.0f;

    const float left=obs.rayDistances[0];
    const float frontLeft=obs.rayDistances[1];
    const float front=obs.rayDistances[2];
    const float frontRight=obs.rayDistances[3];
    const float right=obs.rayDistances[4];

    const float leftClearance=left+frontLeft;
    const float rightClearance=right+frontRight;

    if(front<SAFE_DISTANCE){
        if(lastTurn==Action::LEFT || lastTurn==Action::RIGHT){
            return lastTurn;
        }

        lastTurn=(leftClearance>=rightClearance)
            ? Action::LEFT
            : Action::RIGHT;

        return lastTurn;
    }

    if(frontLeft<SAFE_DISTANCE*0.7f && frontRight>=SAFE_DISTANCE*0.7f){
        lastTurn=Action::RIGHT;
        return Action::RIGHT;
    }

    if(frontRight<SAFE_DISTANCE*0.7f && frontLeft>=SAFE_DISTANCE*0.7f){
        lastTurn=Action::LEFT;
        return Action::LEFT;
    }

    return Action::FORWARD;
}