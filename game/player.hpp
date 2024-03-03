#pragma once
#ifndef __PLAYER_
#define __PLAYER_

#include "../engine/actor.hpp"
#include "../engine/particle.hpp"
#include "../engine/camera.hpp"

class Player : public Actor {
private:
    Particle mPhysics;
    Camera mCamera;
    RVec mLocalCameraPlace;
    
public:
    Player(RVec pos = RVec(0.0f)) {
        mPhysics.SetMass(65.0f);
        mPhysics.mPosition = pos;
    }
};

#endif