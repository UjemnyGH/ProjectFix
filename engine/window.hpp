#pragma once
#ifndef __WINDOW_
#define __WINDOW_

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "core.hpp"
#include <thread>
#include "renderer.hpp"
#include "ui.hpp"
#include "scene.hpp"
#include <vector>
#include <string>

class Window {
private:
    static Window* pWindow;

    static void sWindowPressedCharacterCallback(GLFWwindow* wnd, uint32_t code) {
        wnd = wnd;

        gWindowPressedCharacter = code;
    }

    static void sWindowDragAndDropCallback(GLFWwindow* wnd, int count, const char** paths) {
        gDropFilePaths.clear();

        for(int i = 0; i < count; i++) {
            gDropFilePaths.push_back(std::string(paths[i]));
        }
    }

    static void sWindowErrorCallback(int error_code, const char* description) {
        ENG_ERR("GLFW: " << error_code << "\t Description: " << description)
    }

    static void sFramebufferSizeCallback(GLFWwindow* wnd, int width, int height) {
        ENG_INFO("Window resized to new size " << width << "x" << height);
        wnd = wnd;

        glViewport(0, 0, width, height);

        TextRenderer::SetWindowSize(width, height);
        WorldTextRenderer::SetWindowSize(width, height);

        gSceneHandler.WindowSizeScene(width, height);

        pWindow->WindowSize(width, height);
    }

    static void sCursorCallback(GLFWwindow* wnd, double width, double height) {
        wnd = wnd;

        UIComponent::PassCursorPos(width, height);

        gSceneHandler.MousePositionScene(width, height);

        pWindow->MousePosition(width, height);
    }

    double mFixedTime, mFixedLast, mFixedDelta;

    void fixedUpdateThreadCallback() {
        while(true) {
            auto start = std::chrono::high_resolution_clock::now();
            mFixedTime = std::chrono::duration<double>(start.time_since_epoch()).count();
            mFixedDelta = mFixedTime - mFixedLast;
            mFixedLast = mFixedTime;

            //if(mFixedDelta == 0.0) { continue; }

            gSceneHandler.FixedUpdateScene(mFixedDelta);

            FixedUpdate(mFixedDelta);

            while(std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start).count() < 1.0 / mUpdatesPerSec) continue;
        }
    }

    GLFWwindow* mWindow;

public:
    static std::vector<std::string> gDropFilePaths;
    static uint32_t gWindowPressedCharacter;

    Window() {
        if(pWindow == nullptr) {
            Window::pWindow = this;
        }
        else {
            ENG_ERR_C("Window exist, there is no way you creating another one!");
        }
    }

    int mMultisamplerSamples = 4;
    double mUpdatesPerSec = 128.0;

    enum {
        S_PRESS = GLFW_PRESS,
        S_RELEASE = GLFW_RELEASE,
        S_REPEAT = GLFW_REPEAT
    };

    virtual void Awake() {}
    virtual void Start() {}
    virtual void Update(double) {}
    virtual void LateUpdate(double) {}
    virtual void FixedUpdate(double) {}
    virtual void WindowSize(int, int) {}
    virtual void MousePosition(double, double) {}

    GLFWwindow* GetWindow() { return mWindow; }
    static Window* GetMainWindow() { return pWindow; }
    static uint32_t GetChar() { return gWindowPressedCharacter; }

    inline void DisableCursor() { glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED); }
    inline void EnableCursor() { glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL); }
    inline void HideCursor() { glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); }
    inline void SetCursorPosition(double x, double y) { glfwSetCursorPos(mWindow, x, y); }
    inline bool Key(int key, int state = S_PRESS) { return glfwGetKey(mWindow, key) == state; }

    void Run() {
        Awake();

        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, mMultisamplerSamples);

        mWindow = glfwCreateWindow(800, 600, "Ordinary engine", nullptr, nullptr);

        if(!mWindow) {
            ENG_ERR_C("Cannot create window");
        }
        else {
            ENG_DEBUG("Window created successfully")
        }

        glfwMakeContextCurrent(mWindow);
        glfwSetErrorCallback(Window::sWindowErrorCallback);

        if(!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
            ENG_ERR_C("Cannot load OpenGL functions")
        }
        else {
            ENG_DEBUG("OpenGL context loaded successfully")
        }

        glfwSetFramebufferSizeCallback(mWindow, Window::sFramebufferSizeCallback);
        Window::sFramebufferSizeCallback(mWindow, 800, 600);
        glfwSetCursorPosCallback(mWindow, Window::sCursorCallback);
        glfwSetCharCallback(mWindow, Window::sWindowPressedCharacterCallback);
        glfwSetDropCallback(mWindow, Window::sWindowDragAndDropCallback);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_FRAMEBUFFER_SRGB);

        _debug_Init();

        Start();

        double last = 0.0, current = 0.0, delta = 0.0;

        std::jthread fixedUpdateThread(&Window::fixedUpdateThreadCallback, this);
        fixedUpdateThread.detach();

        while(!glfwWindowShouldClose(mWindow)) {
            current = glfwGetTime();
            delta = current - last;
            last = current;

            UIComponent::CheckMouseClick(mWindow);

            glClear(0x100 | 0x4000);
            glClearColor(0.01f, 0.01f, 0.01f, 1.0f);

            gSceneHandler.UpdateScene(delta);

            Update(delta);

            glfwSwapBuffers(mWindow);

            //ENG_DEBUG("GLFW swapped buffers")

            gSceneHandler.LateUpdateScene(delta);

            LateUpdate(delta);

            glfwPollEvents();
        }

        fixedUpdateThread.request_stop();

        glfwTerminate();
    }

    ~Window() {
        if(Window::pWindow != nullptr) {
            Window::pWindow = nullptr;
        }
    }
};

Window* Window::pWindow = nullptr;
uint32_t Window::gWindowPressedCharacter = 0;
std::vector<std::string> Window::gDropFilePaths = std::vector<std::string>();

#endif