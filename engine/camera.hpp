#pragma once
#ifndef __CAMERA_
#define __CAMERA_

#include "engine_math.hpp"
#include "ui.hpp"

RMat gDefaultViewMatrix = RMat::Identity();
RMat gDefaultProjectionMatrix = RMat::Identity();

class Camera : public EObject{
private:
    static uint32_t gCamerasCount;
    int mLastMouseX;
    int mLastMouseY;
    double mCameraSensitivity = 0.005;
    RVec mCameraEnd;
    double mCameraYRange = 89.9;

public:
    RVec mPosition = RVec();
    RVec mDirection = RVec(0.0f, 0.0f, 1.0f);
    RVec mFront = RVec(0.0f, 0.0f, 1.0f);
    RVec mRight = RVec(1.0f, 0.0f, 0.0f);
    RMat mViewMatrix = RMat::Identity();

    static std::vector<Camera*> gpCameras;

    Camera() : EObject("Camera" + std::to_string(Camera::gCamerasCount), "CAMERA_CLASS") { Camera::gCamerasCount++; Camera::gpCameras.push_back(this); mCommonType = EObjectCommonType_Camera;  }

    RMat operator()() { return mViewMatrix; }

    virtual void ShouldLateUpdate() override {
        CalculateLookAtMatrix();
        CalculateDirection(UIComponent::mMouseX, UIComponent::mMouseY);
    }

    void SetLastMouse(int x, int y) {
        mLastMouseX = x;
        mLastMouseY = y;
    }

    void SetCameraSensitivity(double sensitivity) {
        mCameraSensitivity = (sensitivity / 100.0f);
    }

    void SetCameraYRange(double range) {
        mCameraYRange = range;
    }

    void CalculateDirection(double mouseX, double mouseY) {
        float offsetX = mouseX - mLastMouseX;
        float offsetY = mLastMouseY - mouseY;
        
        mLastMouseX = mouseX;
        mLastMouseY = mouseY;

        offsetX *= mCameraSensitivity;
        offsetY *= mCameraSensitivity;

        mCameraEnd.x += offsetX;
        mCameraEnd.y -= offsetY;

        mCameraEnd.y = std::clamp(mCameraEnd.y, ToRadians(-mCameraYRange), ToRadians(mCameraYRange));

        mDirection.x = cos(mCameraEnd.y) * cos(mCameraEnd.x);
        mDirection.y = sin(mCameraEnd.y);
        mDirection.z = cos(mCameraEnd.y) * sin(mCameraEnd.x);
        mDirection.w = 0.0f;
    }

    void CalculateLookAtMatrix() {
        mDirection.w = 0.0f;
        mFront = mDirection.Normalize();
        mRight = mDirection.Normalize().Cross(RVec(0.0f, 1.0f, 0.0f)).Normalize();

        mFront = RVec(mFront.x, mFront.y, mFront.z);
        mRight = RVec(mRight.x, mRight.y, mRight.z);

        mViewMatrix = RMat::LookAt(mPosition - mFront, mPosition, RVec(0.0f, 1.0f, 0.0f));
    }

    void CalculateTransformMatrix() {
        mFront = mDirection.Normalize();
        mRight = mFront.Cross(RVec(0.0f, 1.0f, 0.0f)).Normalize();

        mFront = RVec(mFront.x, mFront.y, mFront.z);
        mRight = RVec(mRight.x, mRight.y, mRight.z);

        mViewMatrix = RMat::Translate(mPosition) * RMat::RotateY(atan2(mFront.x, mFront.z)) * RMat::RotateX(mFront.y);
    }
};

uint32_t Camera::gCamerasCount = 0;
std::vector<Camera*> Camera::gpCameras = std::vector<Camera*>();

#endif