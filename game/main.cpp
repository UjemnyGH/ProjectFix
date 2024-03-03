//#define DEBUG
#define DEBUG_DRAW
//#define HIDE_INFO

#include <iostream>
#include "../engine/window.hpp"
#include "../engine/bitmap_images.hpp"
#include "../engine/renderer.hpp"
#include "../engine/camera.hpp"
#include "../engine/lights.hpp"
#include "../engine/collisions.hpp"
#include "../engine/particle.hpp"
#include "../engine/ui.hpp"
#include "../engine/data_files.hpp"

#include "vehicle.hpp"

#include "title_scene.hpp"
#include "test_game_scene.hpp"

class Wnd : public Window {
public:
    virtual void Start() override;
};

DataFile mProjectMain;

void Wnd::Start() {
    gSceneHandler.SetSceneByName("TitleScene");
}

int main() {
    Wnd wnd;

    wnd.Run();

    return 0;
}

/*
//#define DEBUG
#define DEBUG_DRAW
//#define HIDE_INFO

#include <iostream>
#include "../engine/window.hpp"
#include "../engine/bitmap_images.hpp"
#include "../engine/renderer.hpp"
#include "../engine/camera.hpp"
#include "../engine/lights.hpp"
#include "../engine/collisions.hpp"
#include "../engine/particle.hpp"
#include "../engine/ui.hpp"

#include "vehicle.hpp"

#include "title_scene.hpp"

class Wnd : public Window {
public:
    virtual void Awake() override;
    virtual void Start() override;
    virtual void Update(double dt) override;
    virtual void LateUpdate(double dt) override;
    virtual void FixedUpdate(double dt) override;
    virtual void WindowSize(int width, int height) override;
    virtual void MousePosition(double x, double y) override;
};

Renderer renderer;
Renderer testVehicle;
Camera mPlayerCamera;
bool gPlayerOnGround = false;
bool gKeyPressed = false;
DirLight gDirLight;
Material gMaterial;
SpotLight gFlashlight;
PointLight gPointLight;
Particle mPlayerParticle;
ParticleGravity mPGravity;
FrameBuffer mShadowFBO;

MeshData terrain = MeshData::LoadOBJModelTriangulated(MODEL_ASSET(terrain1.obj));
MeshData cube = MeshData::LoadOBJModelTriangulated(MODEL_ASSET(cube_triangl.obj));
Vehicle mTestVehicle;

TextRenderer text;
WorldTextRenderer worldText;

Button button;

void Wnd::Awake() {
    terrain.SetColor(RVec(0.6f, 0.6f, 0.6f, 1.0f));
    mPlayerParticle.SetMass(65.0f);
    mPlayerParticle.mPosition = RVec(0.0f, 160.0f, 0.0f);
    mPGravity.mGravity = RVec(0.0f, -20.0f);

    gDirLight.mAmbient = RVec(0.00001f, 0.00001f, 0.00001f);
    gDirLight.mDiffuse = RVec(0.6f, 0.6f, 0.6f);
    gDirLight.mSpecular = RVec(0.1f, 0.1f, 0.1f);
    gDirLight.mDirection = RVec(-0.1f, -1.0f, 0.0f);

    gMaterial.mAmbient = RVec(0.00001f, 0.00001f, 0.00001f);
    gMaterial.mShininess = 64.0f;

    gPointLight.mAmbient = RVec(0.1f, 0.1f, 0.1f);
    gPointLight.mDiffuse = RVec(1.0f, 1.0f, 1.0f);
    gPointLight.mSpecular = RVec(0.7f, 0.7f, 0.7f);
    gPointLight.mPosition = RVec(0.0f, 0.0f, 0.0f);
    gPointLight.mContrast = 20.0f;
    gPointLight.mLinear = 9.0f;
    gPointLight.mQuadratic = 30.0f;

    //gPointLight.mContrast = 1.0f;
    //gPointLight.mLinear = 0.9f;
    //gPointLight.mQuadratic = 0.3f;

    gFlashlight.mAmbient = RVec(0.02f, 0.02f, 0.02f);
    gFlashlight.mDiffuse = RVec(0.5f, 0.5f, 0.4f);
    gFlashlight.mSpecular = RVec(0.8f, 0.8f, 0.8f);
    gFlashlight.mInnerCutoff = ToRadians(12.5f);
    gFlashlight.mOuterCutoff = ToRadians(17.5f);
}

Shader color_vs;
Shader color_fs;

void Wnd::Start() {
    gSceneHandler.AddScene(&gTitleScene);
    gSceneHandler.SetSceneByName("TitleScene");

    mTestVehicle = Vehicle(MODEL_ASSET(test_car.obj), MODEL_ASSET(centered_wheelR15.obj));

    mShadowFBO.Init();

    color_vs.FileShader(SHADER_ASSET(light.vert), GL_VERTEX_SHADER);
    color_fs.FileShader(SHADER_ASSET(light.frag), GL_FRAGMENT_SHADER);

    renderer.Setup();
    testVehicle.Setup();

    renderer.AddShader(&color_vs);
    renderer.AddShader(&color_fs);
    renderer.ReattachShaders();

    testVehicle.AddShader(&color_vs);
    testVehicle.AddShader(&color_fs);
    testVehicle.ReattachShaders();

    testVehicle.AddData(*mTestVehicle.GetVehicleMesh());
    testVehicle.AddData(*mTestVehicle.GetVehicleMesh());
    testVehicle.AddData(*mTestVehicle.GetVehicleMesh());
    testVehicle.AddData(*mTestVehicle.GetVehicleMesh());
    testVehicle.AddData(*mTestVehicle.GetVehicleMesh());

    uint32_t w = 0, h = 0;
    std::vector<uint8_t> difGrass = LoadBitmapFromFile(TEXTURE_ASSET(difGrass1024.bmp), &w, &h);
    std::vector<uint8_t> specGrass = LoadBitmapFromFile(TEXTURE_ASSET(specGrass1024.bmp), &w, &h);
    renderer.GetTextureID(0)->BindData(JoinToVector<uint8_t>({difGrass, specGrass}), w, h, false, 2);
    //renderer.GetTextureID(0)->BindData(testImage, w, h, true, 1);
    renderer.AddData(terrain);
    renderer.AddData(cube);
    renderer.GetRenderData()->SetTextureID(0, 0);
    renderer.GetRenderData()->SetTextureCoordinatesWrapping(0, RVec(0.001f, 0.001f));

    text.Setup();
    worldText.Setup();
    button.Setup();
    button.SetButton(RVec(-0.8f, -0.8f), RVec(0.1f, 0.1f));
    button.SetBGColor(RVec(0.8f));
    button.SetText("\x1b[fffButton");
    button.GetText()->mPointSize = 4.0f;

    mTestVehicle.GetTransform()->SetPosition(RVec(10.0f, 160.0f, 10.0f));
    mTestVehicle.GetTransform()->SetScale(RVec(3.0f, 3.0f, 3.0f));
    testVehicle.GetRenderData()->mTransform[0] = *mTestVehicle.GetTransform();
    testVehicle.GetRenderData()->mTransform[1] = *mTestVehicle.GetTransform();
    testVehicle.GetRenderData()->mTransform[2] = *mTestVehicle.GetTransform();
    testVehicle.GetRenderData()->mTransform[3] = *mTestVehicle.GetTransform();
    testVehicle.GetRenderData()->mTransform[4] = *mTestVehicle.GetTransform();
    testVehicle.GetRenderData()->TransformAllData();
}

double gFixedDeltaTime = 0.0f;

Box player_collision_box;

void Wnd::Update(double dt) {
    dt = dt;

    //gDirLight.mDirection = RVec(-0.5f, sin(glfwGetTime()), 0.0f);

    if(glfwGetKey(GetWindow(), GLFW_KEY_L) == GLFW_PRESS) {
        color_vs.~Shader();
        color_fs.~Shader();

        color_vs.FileShader(SHADER_ASSET(light.vert), GL_VERTEX_SHADER);
        color_fs.FileShader(SHADER_ASSET(light.frag), GL_FRAGMENT_SHADER);

        renderer.ReattachShaders();
    }

    text.AddText("FPS: " + std::to_string((int)(1.0f / dt)));
    text.AddText("\n\x1b[f00Fixed FPS: " + std::to_string((int)(1.0f / gFixedDeltaTime)));
    text.AddText("\n\x1b[0f0X: " + std::to_string(mPlayerParticle.mPosition.x)); 
    text.AddText("\tY: " + std::to_string(mPlayerParticle.mPosition.y));
    text.AddText("\tZ: " + std::to_string(mPlayerParticle.mPosition.z));
    text.AddText("\n\x1b[00fVX: " + std::to_string(mPlayerParticle.mVelocity.x));
    text.AddText("\tVY: " + std::to_string(mPlayerParticle.mVelocity.y));
    text.AddText("\tVZ: " + std::to_string(mPlayerParticle.mVelocity.z));
    text.AddText("\nScreen space mouse: X = " + std::to_string((UIComponent::mMouseX / TextRenderer::mWidth) * 2.0f - 1.0f) + ", Y = " + std::to_string(-(UIComponent::mMouseY / TextRenderer::mHeight) * 2.0f + 1.0f));

    renderer.RebindBuffers();
    testVehicle.RebindBuffers();

    Material::Pass(&renderer, gMaterial);
    DirLight::Pass(&renderer, gDirLight);
    //PointLight::Pass(&renderer, &gPointLight, 1);
    renderer.GetShaderProgramPtr()->Use();
    renderer.GetShaderProgramPtr()->SetFloat1("u_Gamma", 2.2f);
    renderer.GetShaderProgramPtr()->SetFloat3("u_ViewPosition", mPlayerCamera.mPosition.x, mPlayerCamera.mPosition.y, mPlayerCamera.mPosition.z);
    gFlashlight.mPosition = mPlayerCamera.mPosition;
    gFlashlight.mDirection = mPlayerCamera.mFront;
    renderer.GetShaderProgramPtr()->Unuse();
    //SpotLight::Pass(&renderer, &gFlashlight, 1);

    //mShadowFBO.Bind();
    //renderer.Render();
    //mShadowFBO.GetTextureFromFrameDepth(1024, 1024);
    //renderer.GetTextureID(31)->mID = mShadowFBO.mTextureID;
    renderer.Render();

    Material::Pass(&testVehicle, gMaterial);
    DirLight::Pass(&testVehicle, gDirLight);
    testVehicle.GetShaderProgramPtr()->Use();
    testVehicle.GetShaderProgramPtr()->SetFloat1("u_Gamma", 2.2f);
    testVehicle.GetShaderProgramPtr()->SetFloat3("u_ViewPosition", mPlayerCamera.mPosition.x, mPlayerCamera.mPosition.y, mPlayerCamera.mPosition.z);
    testVehicle.GetShaderProgramPtr()->Unuse();
    testVehicle.Render();

    text.UpdateTextVertices(RVec(-0.95f, 0.95f));
    text.Render();

    worldText.AddText("Hello");

    worldText.GetTransform()->SetPosition(RVec(0.0f, 165.0f, 0.0f));
    worldText.GetTransform()->SetScale(RVec(2.0f, 2.0f, 2.0f));

    worldText.UpdateTextVertices(RVec(0.0f), RVec(1.0f));

    worldText.Render();

    player_collision_box._debug_Show();

    mTestVehicle._debug_ShowTriggers();

    button.RebindData();
    button.Render();

    glfwSwapInterval(1);
}

void Wnd::LateUpdate(double dt) {
    gKeyPressed = false;

    if(Key('W')) {
        if(gPlayerOnGround) mPlayerParticle.mVelocity -= RVec(mPlayerCamera.mFront.x, 0.0f, mPlayerCamera.mFront.z, 0.0f).Normalize() * 8.0f;
        else mPlayerParticle.mVelocity -= RVec(mPlayerCamera.mFront.x, 0.0f, mPlayerCamera.mFront.z, 0.0f).Normalize() * 0.8f;
        gKeyPressed = true;
    }
    else if(Key('S')) {
        if(gPlayerOnGround) mPlayerParticle.mVelocity += RVec(mPlayerCamera.mFront.x, 0.0f, mPlayerCamera.mFront.z, 0.0f).Normalize() * 8.0f;
        else mPlayerParticle.mVelocity += RVec(mPlayerCamera.mFront.x, 0.0f, mPlayerCamera.mFront.z, 0.0f).Normalize() * 0.8f;
        gKeyPressed = true;
    }

    if(Key('A')) {
        if(gPlayerOnGround) mPlayerParticle.mVelocity += mPlayerCamera.mRight.Normalize() * 8.0f;
        else mPlayerParticle.mVelocity += mPlayerCamera.mRight.Normalize() * 0.8f;
        gKeyPressed = true;
    }
    else if(Key('D')) {
        if(gPlayerOnGround) mPlayerParticle.mVelocity -= mPlayerCamera.mRight.Normalize() * 8.0f;
        else mPlayerParticle.mVelocity -= mPlayerCamera.mRight.Normalize() * 0.8f;
        gKeyPressed = true;
    }

    if(Key(' ') && gPlayerOnGround) {
        mPlayerParticle.mVelocity.y += 10.0f;
        gKeyPressed = true;
    }

    if(Key('Q')) {
        mPlayerParticle.mVelocity -= RVec(0.0f, 2.0f, 0.0f, 0.0f);
        gKeyPressed = true;
    }
    else if(Key('E')) {
        mPlayerParticle.mVelocity += RVec(0.0f, 2.0f, 0.0f, 0.0f);
        gKeyPressed = true;
    }

    if(Key('R')) {
        mPlayerParticle.mPosition = RVec(0.0f, 165.0f, 0.0f);
        mPlayerParticle.mVelocity = RVec(0.0f, 0.0f, 0.0f);
    }

    if(Key('C')) {
        mPlayerParticle.mPosition = RVec(0.0f, 0.0f, 0.0f);
        mPlayerParticle.mVelocity = RVec(0.0f, 0.0f, 0.0f);
    }

    if(gPlayerOnGround && !Key('Q')) {
        real velo = (std::abs(mPlayerParticle.mVelocity.x) + std::abs(mPlayerParticle.mVelocity.z)) / 2.0f;
        if(velo > 15.0 || velo < -15.0) {
            mPlayerParticle.mVelocity = (RVec(mPlayerParticle.mVelocity.x, 0.0, mPlayerParticle.mVelocity.z).Normalize() * 15.0).Clamp(-15.0f, 15.0f) + RVec(0.0f, mPlayerParticle.mVelocity.y, 0.0f);
        }
    }

    mPlayerCamera.mPosition = mPlayerParticle.mPosition + RVec(0.0f, 2.5f, 0.0f);

    mPlayerCamera.CalculateLookAtMatrix();
    gDefaultViewMatrix = mPlayerCamera();
    worldText.SetViewMatrix(gDefaultViewMatrix);
}

void Wnd::FixedUpdate(double dt) {
    gFixedDeltaTime = dt;

    if(!gKeyPressed) {
        mPlayerParticle.mVelocity *= RVec(0.8f, 1.0f, 0.8f, 0.0f);
    }

    mPGravity.UpdateForce(&mPlayerParticle, dt);

    for(size_t i = 0; i < renderer.GetRenderData()->mTransformedData[0].mVertices.size() / 9; i++) {
        RVec p1 = RVec(renderer.GetRenderData()->mTransformedData[0].mVertices[i * 9 + 0], renderer.GetRenderData()->mTransformedData[0].mVertices[i * 9 + 1], renderer.GetRenderData()->mTransformedData[0].mVertices[i * 9 + 2]);
        RVec p2 = RVec(renderer.GetRenderData()->mTransformedData[0].mVertices[i * 9 + 3], renderer.GetRenderData()->mTransformedData[0].mVertices[i * 9 + 4], renderer.GetRenderData()->mTransformedData[0].mVertices[i * 9 + 5]);
        RVec p3 = RVec(renderer.GetRenderData()->mTransformedData[0].mVertices[i * 9 + 6], renderer.GetRenderData()->mTransformedData[0].mVertices[i * 9 + 7], renderer.GetRenderData()->mTransformedData[0].mVertices[i * 9 + 8]);
        
        player_collision_box.mMax = mPlayerParticle.mPosition + RVec(0.5f, 3.0f, 0.5f);
        player_collision_box.mMin = mPlayerParticle.mPosition - RVec(0.5f, 3.0f, 0.5f);

        Plane current_plane(p1, p2, p3);
        RVec point = PointOnPlane(p1, p2, p3, player_collision_box.GetPosition() - (current_plane.mNormal * player_collision_box.GetSize()));
        //RVec point = PointOnPlane(p1, p2, p3, mPlayerParticle.mPosition - RVec(0.0f, 3.0f));
        
        if(BoxCanBeOnPlane(player_collision_box, current_plane)) {
            if(BoxToPlaneCollision(player_collision_box, current_plane)) {

                Box ground_coll;
                ground_coll.mMin = mPlayerParticle.mPosition - RVec(0.5f, 3.2f, 0.5f);
                ground_coll.mMax = mPlayerParticle.mPosition - RVec(-0.5f, 2.8f, -0.5f);

                gPlayerOnGround = BoxToPlaneCollision(ground_coll, current_plane);

                if(!ResolveVelocity(dt, current_plane.mNormal, &mPlayerParticle)) break;
                if(!ResolvePenetration(dt, point.Distance(RVec(point.x, player_collision_box.mMin.y, point.z)) * dt, current_plane.mNormal, &mPlayerParticle)) break;
                
                gPointLight.mPosition = point + RVec(0.0f, 0.5f);
                renderer.GetRenderData()->mTransform[1].SetPosition(point + RVec(0.0f, 0.2f));
                renderer.GetRenderData()->mTransform[1].SetScale(RVec(0.1f, 0.1f, 0.1f));
                renderer.GetRenderData()->TransformData(1);

                break;
            }
        }
        else {
            gPlayerOnGround = false;
            continue;
        }
    } 

    mPlayerParticle.Integrate(dt);
}

void Wnd::WindowSize(int width, int height) {
    gDefaultProjectionMatrix = RMat::PerspectiveFOV(ToRadians(90.0f), (float)width, (float)height, 0.001f, 1000.0f);
    worldText.SetProjectionMatrix(RMat::PerspectiveFOV(ToRadians(90.0f), width, height, 0.01f, 1000.0f));
}

bool gMouseLocked = false;

void Wnd::MousePosition(double x, double y) {
    if(UIComponent::mMouseLClick && !button.IsHovered()) {
        DisableCursor();

        if(!gMouseLocked) {
            mPlayerCamera.SetLastMouse(100, 100);

            SetCursorPosition(100, 100);

            gMouseLocked = true;
        }

        mPlayerCamera.CalculateDirection(x, y);
    }
    else {
        EnableCursor();

        gMouseLocked = false;
    }
}

int main() {
    Wnd wnd;

    wnd.Run();

    return 0;
}*/