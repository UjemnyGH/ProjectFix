#pragma once
#ifndef __PARTICLE_
#define __PARTICLE_

#include "engine_math.hpp"
#include <vector>

class Particle {
public:
    RVec mPosition = RVec(0.0f);
    RVec mVelocity = RVec(0.0f);
    RVec mAcceleration = RVec(0.0f);
    real mDamping = 0.995f;
    real mInverseMass = 1.0f;
    RVec mForceAccumulation = RVec(0.0f);

    void ClearForceAccumulation() {
        mForceAccumulation = RVec(0.0f);
    }

    void Integrate(real dur) {
        if(dur <= 0.0f) { return; }

        mPosition += mVelocity * dur;

        RVec resultAcceleration = mAcceleration + (mForceAccumulation * mInverseMass);
        mVelocity += resultAcceleration * dur;

        mVelocity *= pow(mDamping, dur);

        ClearForceAccumulation();
    }

    void SetMass(real mass) {
        mInverseMass = 1.0f / mass;
    }

    real GetMass() { return 1.0f / mInverseMass; }

    bool InfiniteMass() { return mInverseMass == 0.0f; }

    void AddForce(RVec force) {
        mForceAccumulation += force;
    }
};

/**
 * @brief Calculate contact normal of 2 patricles
 * 
 * @param p1 
 * @param p2 
 * @return RVec
 */
RVec CalculateContactNormal(Particle* p1, Particle* p2) {
    RVec contact_normal = RVec();
    
    if(p1 != nullptr) contact_normal = p1->mPosition;
    if(p2 != nullptr) contact_normal -= p2->mPosition;

    return contact_normal.Normalize();
}

/**
 * @brief Resolve particle velocity
 * 
 * @param dt delta time
 * @param contactNormal contact normal
 * @param p1 paricle 1
 * @param p2 particle 2
 * @param restitution calculated velocity scale (default 1/3)
 * @return true
 * @return false
 */
bool ResolveVelocity(real dt, RVec contactNormal, Particle* p1, Particle* p2 = nullptr, real restitution = 0.33f) {
    RVec relative_velo = RVec();

    // Calculating relative velocity
    if(p1 != nullptr) relative_velo = p1->mVelocity;
    if(p2 != nullptr) relative_velo -= p2->mVelocity;
    
    // Separating velocity dotted to normal
    real sep_velo = relative_velo.Dot(contactNormal);

    if(sep_velo > 0) return false;

    // Calculating new separating velocity
    real new_sep_velo = -sep_velo * restitution;

    RVec accel_velo;
    // Calctulating acceleration caused velocity
    if(p1 != nullptr) accel_velo = p1->mAcceleration;
    if(p2 != nullptr) accel_velo -= p2->mAcceleration;

    real accel_sep_velo = accel_velo.Dot(contactNormal) * dt;

    if(accel_sep_velo < 0.0) {
        new_sep_velo += restitution * accel_sep_velo;

        if(new_sep_velo < 0.0) new_sep_velo = 0.0f;
    }

    real delta_velo = new_sep_velo - sep_velo;

    // Calculating total mass
    real total_inv_mass = 0.0f;
    if(p1 != nullptr) total_inv_mass = p1->mInverseMass;
    if(p2 != nullptr) total_inv_mass += p2->mInverseMass;
    if(total_inv_mass <= 0.0) return false;

    // Calculating impulse for particles
    real impulse = delta_velo / total_inv_mass;
    RVec impulse_mass = contactNormal * impulse;

    if(p1 != nullptr) p1->mVelocity += impulse_mass * p1->mInverseMass;
    if(p2 != nullptr) p2->mVelocity += impulse_mass * -p2->mInverseMass;

    return true;
}

/**
 * @brief Resolve penetration into to particles
 * 
 * @param dt delta time
 * @param penetrationDepth penmetration depth 
 * @param contactNormal contact normal
 * @param p1 particle 1
 * @param p2 particle 2
 * @return true
 * @return false
 */
bool ResolvePenetration(real dt, real penetrationDepth, RVec contactNormal, Particle* p1, Particle* p2 = nullptr) {
    if(penetrationDepth <= 0.0) return false;

    // Calculating total mass
    real total_inv_mass = 0.0f;
    if(p1 != nullptr) total_inv_mass = p1->mInverseMass;
    if(p2 != nullptr) total_inv_mass += p2->mInverseMass;

    if(total_inv_mass <= 0.0) return false;

    // Calculating moving mass
    RVec move_mass = contactNormal * (penetrationDepth / total_inv_mass);

    if(p1 != nullptr) p1->mPosition += move_mass * p1->mInverseMass;
    if(p2 != nullptr) p2->mPosition += move_mass * p2->mInverseMass;

    return true;
}

class ParticleForceGenerator {
public:
    virtual void UpdateForce(Particle*, real) {}
};

class ParticleGravity : public ParticleForceGenerator {
public:
    RVec mGravity = RVec(0.0f, -10.0f, 0.0f, 0.0f);

    virtual void UpdateForce(Particle* pParticle, real) override {
        if(pParticle->InfiniteMass()) return;

        pParticle->AddForce(mGravity * pParticle->GetMass());
    }
};

class ParticleDrag : public ParticleForceGenerator {
public:
    real mVelocityDragCoeff;
    real mVelocitySquaredDragCoeff;

    virtual void UpdateForce(Particle* pParticle, real) override {
        RVec force = pParticle->mVelocity;

        real dragCoeff = force.Length();
        dragCoeff = mVelocityDragCoeff * dragCoeff + mVelocitySquaredDragCoeff * dragCoeff * dragCoeff;

        force = force.Normalize();
        force *= dragCoeff;
        pParticle->AddForce(force);
    }
};

/*class ParticleForceHandler {
protected:
    struct ForceHandle {
        Particle* pParticle;
        std::vector<
    };

    std::vector<
};*/

#endif