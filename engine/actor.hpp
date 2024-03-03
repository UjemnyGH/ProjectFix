#pragma once
#ifndef __ACTOR_
#define __ACTOR_


#include "mesh.hpp"
#include "engine_math.hpp"
#include "collisions.hpp"
#include "particle.hpp"

class Actor : public EObject {
private:
    static uint32_t gActorsCount;

    MeshData mMesh;
    Box mBasicCollision;
    Particle mParticle;
    Transform mTransform;

public:
    static std::vector<Actor*> gpActors;

    Actor() : EObject("Actor" + std::to_string(Actor::gActorsCount), "ACTOR_CLASS") { Actor::gActorsCount++; Actor::gpActors.push_back(this); mCommonType = EObjectCommonType_Actor; }

    void SetMesh(MeshData mesh) { mMesh = mesh; }
    MeshData* GetMesh() { return &mMesh;}

    void SetTransform(Transform trans) { mTransform = trans; }
    Transform* GetTransform() { return &mTransform; }

    void SetParticle(Particle part) { mParticle = part; }
    Particle* GetParticle() { return &mParticle; }

    void SetCollisionBox(Box b) { mBasicCollision = b; }
    Box* GetCollisionBox() { return &mBasicCollision; }

    // TODO: Mesh collision
    bool MeshCollision(MeshData mesh) {

    }
};

uint32_t Actor::gActorsCount = 0;
std::vector<Actor*> Actor::gpActors = std::vector<Actor*>();

#endif