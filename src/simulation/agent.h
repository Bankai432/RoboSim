#pragma once

#include "environment.h"

class Agent{
    public:
        Action chooseAct(const Observation &obs) const;
    private:
         mutable Action lastTurn=Action::LEFT;
};