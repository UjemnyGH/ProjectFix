#pragma once
#ifndef __SETTINGS_SCENE_

#include "../engine/simple_text.hpp"
#include "../engine/ui.hpp"
#include "../engine/scene.hpp"
#include "../engine/bitmap_images.hpp"
#include "../engine/renderer.hpp"
#include "../engine/camera.hpp"
#include "../engine/lights.hpp"
#include "../engine/collisions.hpp"
#include "../engine/particle.hpp"

#include "vehicle.hpp"

class TestGameScene : public Scene {
private:
    Shader mColorVert, mColorFrag;
    Renderer mTerrainRenderer;
    MeshData mTerrainMesh;
    DirLight mDirLight;
    Material mMaterial;

    Particle mPlayerParticle;
    ParticleGravity mPGravity;
    Camera mPlayerCamera;
    Box mPlayerCollisionBox;
    TextRenderer mText;

    bool mPlayerOnGround = false;
    bool mKeyPressed = false;

    //bool mStartEnded = false;

public:
    TestGameScene() {
        SetSceneName("TestGameScene");
        gSceneHandler.AddScene(this);
    }

    virtual void Awake() override {
        mPlayerParticle.SetMass(65.0f);
        mPlayerParticle.mPosition = RVec(0.0f, 160.0f, 0.0f);
    
        mDirLight.mAmbient = RVec(0.00001f, 0.00001f, 0.00001f);
        mDirLight.mDiffuse = RVec(0.6f, 0.6f, 0.6f);
        mDirLight.mSpecular = RVec(0.1f, 0.1f, 0.1f);
        mDirLight.mDirection = RVec(-0.1f, -1.0f, 0.0f);

        mMaterial.mAmbient = RVec(0.00001f, 0.00001f, 0.00001f);
        mMaterial.mShininess = 64.0f;
    }

    virtual void Start() override {
        WindowSize(TextRenderer::mWidth, TextRenderer::mHeight);

        mTerrainMesh = MeshData::LoadOBJModelTriangulated(MODEL_ASSET(terrain1.obj));
        mTerrainMesh.SetColor(RVec(0.6f, 0.6f, 0.6f, 1.0f));

        mColorVert.FileShader(SHADER_ASSET(light.vert), GL_VERTEX_SHADER);
        mColorFrag.FileShader(SHADER_ASSET(light.frag), GL_FRAGMENT_SHADER);

        mTerrainRenderer.Setup();

        mTerrainRenderer.AddShader(&mColorVert);
        mTerrainRenderer.AddShader(&mColorFrag);
        mTerrainRenderer.ReattachShaders();

        uint32_t w, h;
        std::vector<uint8_t> difGrass = LoadBitmapFromFile(TEXTURE_ASSET(difGrass1024.bmp), &w, &h);
        std::vector<uint8_t> specGrass = LoadBitmapFromFile(TEXTURE_ASSET(specGrass1024.bmp), &w, &h);

        mTerrainRenderer.GetTextureID(0)->BindData(JoinToVector<uint8_t>({difGrass, specGrass}), w, h, false, 2);
        mTerrainRenderer.AddData(mTerrainMesh);
        mTerrainRenderer.GetRenderData()->SetTextureID(0, 0);
        mTerrainRenderer.GetRenderData()->SetTextureCoordinatesWrapping(0, RVec(0.001f, 0.001f));

        mTerrainRenderer.GetRenderData()->RejoinData();
        mTerrainRenderer.RebindBuffers();

        mText.Setup();

        //mStartEnded = true;
    }

    virtual void Update(double dt) override {

        mText.AddText("FPS: " + std::to_string((int)(1.0f / dt)));
        mText.AddText("\n\x1b[0f0X: " + std::to_string(mPlayerParticle.mPosition.x)); 
        mText.AddText("\tY: " + std::to_string(mPlayerParticle.mPosition.y));
        mText.AddText("\tZ: " + std::to_string(mPlayerParticle.mPosition.z));
        mText.AddText("\n\x1b[00fVX: " + std::to_string(mPlayerParticle.mVelocity.x));
        mText.AddText("\tVY: " + std::to_string(mPlayerParticle.mVelocity.y));
        mText.AddText("\tVZ: " + std::to_string(mPlayerParticle.mVelocity.z));
        mText.AddText("\nScreen space mouse: X = " + std::to_string((UIComponent::mMouseX / TextRenderer::mWidth) * 2.0f - 1.0f) + ", Y = " + std::to_string(-(UIComponent::mMouseY / TextRenderer::mHeight) * 2.0f + 1.0f));

        mTerrainRenderer.RebindBuffers();

        Material::Pass(&mTerrainRenderer, mMaterial);
        DirLight::Pass(&mTerrainRenderer, mDirLight);
        //PointLight::Pass(&renderer, &gPointLight, 1);
        mTerrainRenderer.GetShaderProgramPtr()->Use();
        mTerrainRenderer.GetShaderProgramPtr()->SetFloat1("u_Gamma", 2.2f);
        mTerrainRenderer.GetShaderProgramPtr()->SetFloat3("u_ViewPosition", mPlayerCamera.mPosition.x, mPlayerCamera.mPosition.y, mPlayerCamera.mPosition.z);
        mTerrainRenderer.GetShaderProgramPtr()->Unuse();
        //SpotLight::Pass(&renderer, &gFlashlight, 1);

        //mShadowFBO.Bind();
        //renderer.Render();
        //mShadowFBO.GetTextureFromFrameDepth(1024, 1024);
        //renderer.GetTextureID(31)->mID = mShadowFBO.mTextureID;
        mTerrainRenderer.Render();

        mText.UpdateTextVertices(RVec(-0.95f, 0.95f));
        mText.Render();

        mPlayerCollisionBox._debug_Show();
    }

    virtual void LateUpdate(double dt) override {
        mKeyPressed = false;

        if(Window::GetMainWindow()->Key('W')) {
            if(mPlayerOnGround) mPlayerParticle.mVelocity -= RVec(mPlayerCamera.mFront.x, 0.0f, mPlayerCamera.mFront.z, 0.0f).Normalize() * 8.0f;
            else mPlayerParticle.mVelocity -= RVec(mPlayerCamera.mFront.x, 0.0f, mPlayerCamera.mFront.z, 0.0f).Normalize() * 0.8f;
            mKeyPressed = true;
        }
        else if(Window::GetMainWindow()->Key('S')) {
            if(mPlayerOnGround) mPlayerParticle.mVelocity += RVec(mPlayerCamera.mFront.x, 0.0f, mPlayerCamera.mFront.z, 0.0f).Normalize() * 8.0f;
            else mPlayerParticle.mVelocity += RVec(mPlayerCamera.mFront.x, 0.0f, mPlayerCamera.mFront.z, 0.0f).Normalize() * 0.8f;
            mKeyPressed = true;
        }

        if(Window::GetMainWindow()->Key('A')) {
            if(mPlayerOnGround) mPlayerParticle.mVelocity += mPlayerCamera.mRight.Normalize() * 8.0f;
            else mPlayerParticle.mVelocity += mPlayerCamera.mRight.Normalize() * 0.8f;
            mKeyPressed = true;
        }
        else if(Window::GetMainWindow()->Key('D')) {
            if(mPlayerOnGround) mPlayerParticle.mVelocity -= mPlayerCamera.mRight.Normalize() * 8.0f;
            else mPlayerParticle.mVelocity -= mPlayerCamera.mRight.Normalize() * 0.8f;
            mKeyPressed = true;
        }

        if(Window::GetMainWindow()->Key(' ') && mPlayerOnGround) {
            mPlayerParticle.mVelocity.y += 10.0f;
            mKeyPressed = true;
        }

        if(Window::GetMainWindow()->Key('Q')) {
            mPlayerParticle.mVelocity -= RVec(0.0f, 2.0f, 0.0f, 0.0f);
            mKeyPressed = true;
        }
        else if(Window::GetMainWindow()->Key('E')) {
            mPlayerParticle.mVelocity += RVec(0.0f, 2.0f, 0.0f, 0.0f);
            mKeyPressed = true;
        }

        if(Window::GetMainWindow()->Key('R')) {
            mPlayerParticle.mPosition = RVec(0.0f, 165.0f, 0.0f);
            mPlayerParticle.mVelocity = RVec(0.0f, 0.0f, 0.0f);
        }

        if(Window::GetMainWindow()->Key('C')) {
            mPlayerParticle.mPosition = RVec(0.0f, 0.0f, 0.0f);
            mPlayerParticle.mVelocity = RVec(0.0f, 0.0f, 0.0f);
        }

        if(mPlayerOnGround && !Window::GetMainWindow()->Key('Q')) {
            real velo = (std::abs(mPlayerParticle.mVelocity.x) + std::abs(mPlayerParticle.mVelocity.z)) / 2.0f;
            if(velo > 15.0 || velo < -15.0) {
                mPlayerParticle.mVelocity = (RVec(mPlayerParticle.mVelocity.x, 0.0, mPlayerParticle.mVelocity.z).Normalize() * 15.0).Clamp(-15.0f, 15.0f) + RVec(0.0f, mPlayerParticle.mVelocity.y, 0.0f);
            }
        }

        mPlayerCamera.mPosition = mPlayerParticle.mPosition + RVec(0.0f, 2.5f, 0.0f);

        mPlayerCamera.CalculateLookAtMatrix();
        gDefaultViewMatrix = mPlayerCamera();
    }

    virtual void FixedUpdate(double dt) override {
        //if(!mStartEnded) return;

        if(!mKeyPressed) {
            mPlayerParticle.mVelocity *= RVec(0.8f, 1.0f, 0.8f, 0.0f);
        }

        mPGravity.UpdateForce(&mPlayerParticle, dt);

        for(size_t i = 0; i < mTerrainRenderer.GetRenderData()->mTransformedData[0].mVertices.size() / 9; i++) {
            RVec p1 = RVec(mTerrainRenderer.GetRenderData()->mTransformedData[0].mVertices[i * 9 + 0], mTerrainRenderer.GetRenderData()->mTransformedData[0].mVertices[i * 9 + 1], mTerrainRenderer.GetRenderData()->mTransformedData[0].mVertices[i * 9 + 2]);
            RVec p2 = RVec(mTerrainRenderer.GetRenderData()->mTransformedData[0].mVertices[i * 9 + 3], mTerrainRenderer.GetRenderData()->mTransformedData[0].mVertices[i * 9 + 4], mTerrainRenderer.GetRenderData()->mTransformedData[0].mVertices[i * 9 + 5]);
            RVec p3 = RVec(mTerrainRenderer.GetRenderData()->mTransformedData[0].mVertices[i * 9 + 6], mTerrainRenderer.GetRenderData()->mTransformedData[0].mVertices[i * 9 + 7], mTerrainRenderer.GetRenderData()->mTransformedData[0].mVertices[i * 9 + 8]);
            
            mPlayerCollisionBox.mMax = mPlayerParticle.mPosition + RVec(0.5f, 3.0f, 0.5f);
            mPlayerCollisionBox.mMin = mPlayerParticle.mPosition - RVec(0.5f, 3.0f, 0.5f);

            Plane current_plane(p1, p2, p3);
            RVec point = PointOnPlane(p1, p2, p3, mPlayerCollisionBox.GetPosition() - (current_plane.mNormal * mPlayerCollisionBox.GetSize()));
            //RVec point = PointOnPlane(p1, p2, p3, mPlayerParticle.mPosition - RVec(0.0f, 3.0f));
            
            if(BoxCanBeOnPlane(mPlayerCollisionBox, current_plane)) {
                if(BoxToPlaneCollision(mPlayerCollisionBox, current_plane)) {

                    Box ground_coll;
                    ground_coll.mMin = mPlayerParticle.mPosition - RVec(0.5f, 3.2f, 0.5f);
                    ground_coll.mMax = mPlayerParticle.mPosition - RVec(-0.5f, 2.8f, -0.5f);

                    //mPlayerOnGround = BoxToPlaneCollision(ground_coll, current_plane);
                    mPlayerOnGround = true;

                    ResolveVelocity(dt, current_plane.mNormal, &mPlayerParticle);
                    ResolvePenetration(dt, point.Distance(RVec(point.x, mPlayerCollisionBox.mMin.y, point.z)) * dt * 0.1f, current_plane.mNormal, &mPlayerParticle);

                    break;
                }
            }
            else {
                mPlayerOnGround = false;
                continue;
            }
        } 

        mPlayerParticle.Integrate(dt);
    }

    virtual void WindowSize(int width, int height) override {
        gDefaultProjectionMatrix = RMat::PerspectiveFOV(ToRadians(90.0f), (float)width, (float)height, 0.001f, 1000.0f);
    }

    bool gMouseLocked = false;

    virtual void MousePosition(double x, double y) override {
        if(UIComponent::mMouseLClick) {
            Window::GetMainWindow()->DisableCursor();

            if(!gMouseLocked) {
                mPlayerCamera.SetLastMouse(100, 100);

                Window::GetMainWindow()->SetCursorPosition(100, 100);

                gMouseLocked = true;
            }

            mPlayerCamera.CalculateDirection(x, y);
        }
        else {
            Window::GetMainWindow()->EnableCursor();

            gMouseLocked = false;
        }
    }
    
} gTestGameScene;

#endif