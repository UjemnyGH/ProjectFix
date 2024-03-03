#pragma once
#ifndef __RIGIDBODY_
#define __RIGIDBODY_

#include "engine_math.hpp"

class Rigidbody {
private:
    Transform mTransform;
    RVec mPosition = RVec();
    RVec mVelocity = RVec();
    RVec mAcceleration = RVec();
    RVec mForceAccululation = RVec();
    real mDumping = 0.995f;
    real mInverseMass = 1.0f;
    RMat mInverseInertiaTensor;

public:
    void Integrate(real dt) {
        if(dt == 0.0) { return; }

        mTransform.SetPosition(mVelocity * dt);

        RVec resAcc = mAcceleration + (mForceAccululation * mInverseMass);
        mVelocity += resAcc * dt;

        mVelocity *= pow(mDumping, dt);

        mForceAccululation = RVec();
    }

    void SetInertia(RMat inertiaTensor) {
        mInverseInertiaTensor = inertiaTensor.Inverse();
    }

    void SetMass(real mass) {
        mInverseMass = 1.0 / mass;
    }

    real GetMass() {
        return 1.0 / mInverseMass;
    }

    bool InfiniteMass() { return mInverseMass == 0.0; }

    void AddForce(RVec force) {
        mForceAccululation += force;
    }

    Transform* GetTransform() { return &mTransform; }
    RVec* GetPosition() { return &mPosition; }
    RVec* GetVelocity() { return &mVelocity; }
    RVec* GetAcceleration() { return &mAcceleration; } 
    RVec* GetForceAccululation() { return &mForceAccululation; }

    void SetTransform(Transform t) { mTransform = t; }
    void SetPosition(RVec v) { mPosition = v; }
    void SetVelocity(RVec v) { mVelocity = v; }
    void SetAcceleration(RVec v) { mAcceleration = v; } 
    void SetForceAccululation(RVec v) { mForceAccululation = v; }
};

void ResolveVelocityWithPoint(Rigidbody* rb, RVec point, RVec normal) {
    real separating_velo = rb->GetVelocity()->Dot(normal);

    if(separating_velo > 0) {
        return;
    }

    RVec new_velo = *rb->GetVelocity() * -separating_velo;

    rb->SetVelocity(new_velo);
}

#endif