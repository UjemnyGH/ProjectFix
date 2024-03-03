#pragma once
#ifndef __VEHICLE_
#define __VEHICLE_

#include "../engine/mesh.hpp"
#include "../engine/collisions.hpp"
#include "../engine/particle.hpp"
#include "../engine/renderer.hpp"

enum Wheel {
    Wheel_FL = 0,
    Wheel_FR = 1,
    Wheel_RL = 2,
    Wheel_RR = 3,
};

class Vehicle {
private:
    Particle mPhysics;
    RVec mWheels[4];
    MeshData mMesh;
    MeshData mWheelMesh;
    Box mDriverSeat;
    Box mBase;
    Transform mTransform;

public:
    Vehicle() { }

    Vehicle(std::string vehicleMesh, std::string wheelMesh) {
        mMesh = MeshData::LoadOBJModelTriangulated(vehicleMesh);
        mWheelMesh = MeshData::LoadOBJModelTriangulated(wheelMesh);
        mDriverSeat = Box(RVec(-0.80f, 0.70f, -0.55f), RVec(0.5f));
        mWheels[Wheel_FL] = RVec(-1.5f, -0.2f, 2.65f);
        mWheels[Wheel_FR] = RVec(1.5f, -0.2f, 2.65f);
        mWheels[Wheel_RL] = RVec(-1.5f, -0.2f, -2.65f);
        mWheels[Wheel_RR] = RVec(1.5f, -0.2f, -2.65f);
        mBase = Box(RVec(0.0f), RVec(1.678f, 0.1f, 4.42f));
        mTransform = Transform();
    }

    /**
     * @brief Show debug triggers
     * 
     * @param p projection mat
     * @param v view mat
     */
    void _debug_ShowTriggers(RMat p = gDefaultProjectionMatrix, RMat v = gDefaultViewMatrix) {
        _debug_DrawCube(mTransform.GetPosition() + (mDriverSeat.GetPosition() * mTransform.GetScale()), mDriverSeat.GetSize() * mTransform.GetScale(), p, v);
        _debug_DrawCube(mTransform.GetPosition() + (mBase.GetPosition() * mTransform.GetScale()), mBase.GetSize() * mTransform.GetScale(), p, v);
    }

    Particle* GetPhysics() { return &mPhysics; }
    RVec GetWheelPosition(Wheel wheel) { return mWheels[wheel]; }
    MeshData* GetVehicleMesh() { return &mMesh; }
    MeshData* GetWheelMesh() { return &mWheelMesh; }
    Box* GetDriverSeat() { return &mDriverSeat; } 
    Box* GetBase() { return &mBase; }
    Transform* GetTransform() { return &mTransform; } 
};

#endif