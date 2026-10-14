
#include "Agent.h"

Action Agent::chooseAct(const Observation& obs) const{
    constexpr float SAFE_DISTANCE=3.0f;

    const float left=obs.rayDistances[0];
    const float frontLeft=obs.rayDistances[1];
    const float front=obs.rayDistances[2];
    const float frontRight=obs.rayDistances[3];
    const float right=obs.rayDistances[4];

    if(front<SAFE_DISTANCE){
        const float leftClearance=0.4*left+0.6f*frontLeft;
        const float rightClearance=0.4f*right+0.6f*frontRight;

        if(leftClearance>rightClearance) return Action::LEFT;
        return Action::RIGHT;
    }
    if(frontLeft<SAFE_DISTANCE*0.7f && frontRight>=SAFE_DISTANCE*0.7f) return Action::RIGHT;
    if(frontRight<SAFE_DISTANCE*0.7f && frontLeft>=SAFE_DISTANCE*0.7f) return Action::LEFT;
    
    return Action::FORWARD;
}
