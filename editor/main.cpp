#define DEBUG_DRAW
#define DEBUG
#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#include "../engine/window.hpp"
#include "../engine/renderer.hpp"
#include "../engine/camera.hpp"
#include "../engine/data_files.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include "project.hpp"

class Wnd : public Window {
private:    
    float mPerspectiveFOV = 90.0f;
    float mPerspectiveNear = 0.001f;
    float mPerspectiveFar = 1000.0f;

    Camera mEditorCamera;
    bool mEditorCameraLocked = false;

    int mWndWidth = 800;
    int mWndHeight = 600;

    int mGameWndWidth = 0;
    int mGameWndHeight = 0;

    bool mColorScene = true;

    FrameBuffer mFramebuffer;

    EdProject mProject;

    std::string mLoadText;

public:
    virtual void Awake() override {
        
    }

    virtual void Start() override {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        ImGui::StyleColorsClassic();

        ImGui_ImplGlfw_InitForOpenGL(GetWindow(), true);
        ImGui_ImplOpenGL3_Init("#version 450 core");

        mFramebuffer.Init();
    }

    virtual void Update(double dt) override {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();

        mProject.ImGuiWindowHandle();

        if(ImGui::BeginMainMenuBar()) {
            mProject.ImGuiClassHandle();

            if(ImGui::BeginMenu("File")) {
                if(ImGui::MenuItem("New")) mProject.ShowNewProjectPopup();
                if(ImGui::MenuItem("Save as")) mProject.ShowSaveAsProjectPopup();
                if(ImGui::MenuItem("Save")) mProject.ShowSaveProjectPopup();
                if(ImGui::MenuItem("Open")) mProject.ShowOpenProjectPopup();

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        if(!gDropFilePaths.empty()) {
            for(auto p : gDropFilePaths) {
                printf("Path: %s\n", p.c_str());
            }

            gDropFilePaths.clear();
        }

        if(ImGui::Begin("Test", nullptr)) {
            ImGui::InputText("Test", &mLoadText);

            ImGui::End();
        }

        if(ImGui::Begin("Camera", nullptr)) {
            bool editor_camera_action = ImGui::SliderFloat("Editor camera FOV", &mPerspectiveFOV, 0.0f, 180.0f, "%.2f deg");
            editor_camera_action |= ImGui::DragFloat("Editor camera near", &mPerspectiveNear);
            editor_camera_action |= ImGui::DragFloat("Editor camera far", &mPerspectiveFar);

            if(editor_camera_action) {
                gDefaultProjectionMatrix = RMat::PerspectiveFOV(ToRadians(mPerspectiveFOV), (float)mWndWidth, (float)mWndHeight, mPerspectiveNear, mPerspectiveFar);
            }

            ImGui::End();
        }

        if(ImGui::Begin("Editor view", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar)) {
            if(ImGui::BeginMenuBar()) {
                if(ImGui::BeginMenu("View")) {
                    ImGui::MenuItem("Color/Depth", "", &mColorScene);

                    ImGui::End();
                }

                ImGui::EndMenuBar();
            }

            ImGui::BeginChild("EditViewChild");

            mGameWndWidth = (int)ImGui::GetWindowSize().x;
            mGameWndHeight = (int)ImGui::GetWindowSize().y;

            ImGui::Image((ImTextureID)mFramebuffer.mTextureID, ImGui::GetWindowSize(), ImVec2(0, 1), ImVec2(1, 0));

            ImGui::EndChild();

            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        mFramebuffer.Bind();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, mGameWndWidth * 2, mGameWndHeight * 2);
        gDefaultProjectionMatrix = RMat::PerspectiveFOV(ToRadians(mPerspectiveFOV), (float)mGameWndWidth * 2, (float)mGameWndHeight * 2, mPerspectiveNear, mPerspectiveFar);

        _debug_DrawCube(RVec(0.0f), RVec(1.0f));

        if(mColorScene) mFramebuffer.GetTextureFromFrameColor(mGameWndWidth * 2, mGameWndHeight * 2);
        else mFramebuffer.GetTextureFromFrameDepth(mGameWndWidth * 2, mGameWndHeight * 2);
        mFramebuffer.Unbind();
    }

    virtual void LateUpdate(double dt) override {
        if(UIComponent::mMouseRClick) {
            if(Key('S')) {
                mEditorCamera.mPosition += mEditorCamera.mFront * dt * 2.0f;
            }
            else if(Key('W')) {
                mEditorCamera.mPosition -= mEditorCamera.mFront * dt * 2.0f;
            }

            if(Key('A')) {
                mEditorCamera.mPosition += mEditorCamera.mRight * dt * 2.0f;
            }
            else if(Key('D')) {
                mEditorCamera.mPosition -= mEditorCamera.mRight * dt * 2.0f;
            }
        }

        mEditorCamera.CalculateLookAtMatrix();
        gDefaultViewMatrix = mEditorCamera();
    }

    virtual void FixedUpdate(double dt) override {

    }

    virtual void WindowSize(int width, int height) override {
        mWndWidth = width;
        mWndHeight = height;
        gDefaultProjectionMatrix = RMat::PerspectiveFOV(ToRadians(mPerspectiveFOV), (float)width, (float)height, mPerspectiveNear, mPerspectiveFar);
    }

    virtual void MousePosition(double x, double y) override {
        if(UIComponent::mMouseRClick) {
            DisableCursor();
            
            if(!mEditorCameraLocked) {
                mEditorCamera.SetLastMouse(100, 100);

                SetCursorPosition(100, 100);

                mEditorCameraLocked = true;
            }

            mEditorCamera.CalculateDirection(x, y);
        }
        else {
            EnableCursor();

            mEditorCameraLocked = false;
        }
    }
};

int main() {
    Wnd wnd;

    wnd.Run();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}