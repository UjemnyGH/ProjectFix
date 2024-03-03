#pragma once
#ifndef __SCENE_

#include "window.hpp"

class Scene : public EObject {
private:
    static uint32_t gScenesCount;

    bool mSceneActive = false;
    std::string mSceneName;

public:
    static std::vector<Scene*> gpScenes;

    Scene() : EObject("Scene" + std::to_string(Scene::gScenesCount), "SCENE_CLASS") { Scene::gScenesCount++; Scene::gpScenes.push_back(this); mCommonType = EObjectCommonType_Scene; }

    bool mSceneAwakend = false;
    bool mSceneStarted = false;

    virtual void Awake() {}
    virtual void Start() {}
    virtual void Update(double) {}
    virtual void LateUpdate(double) {}
    virtual void FixedUpdate(double) {}

    virtual void WindowSize(int, int) {}
    virtual void MousePosition(double, double) {}

    bool GetState() { return mSceneActive; }
    void SetState(bool sceneActive) { mSceneActive = sceneActive; }

    void SetSceneName(std::string name) { mSceneName = name; }
    std::string GetSceneName() { return mSceneName; }
};

class SceneHandler {
private:
    Scene* mCurrentScene;
    std::vector<Scene*> mScenes;

public:
    void SetScene(Scene* scene) {
        if(mCurrentScene != nullptr) mCurrentScene->SetState(false);

        mCurrentScene = scene;

        if(mCurrentScene != nullptr) {
            mCurrentScene->SetState(true);
        
            if(!mCurrentScene->mSceneAwakend) { 
                mCurrentScene->Awake();
                mCurrentScene->mSceneAwakend = true;

                ENG_DEBUG("Awakend scene \"" << mCurrentScene->GetSceneName() << "\"");
            }

            if(!mCurrentScene->mSceneStarted) { 
                mCurrentScene->Start();
                mCurrentScene->mSceneStarted = true;

                ENG_DEBUG("Started scene \"" << mCurrentScene->GetSceneName() << "\"");
            }
        }
    }

    void AddScene(Scene* scene) {
        mScenes.push_back(scene);

        ENG_DEBUG("Added scene \"" << scene->GetSceneName() << "\"");
    }

    void SetSceneByName(std::string name) {
        int iter = 0;

        for(auto sc : mScenes) {
            if(sc->GetSceneName() == name) {
                break;
            }

            iter++;
        }

        SetScene(mScenes[iter]);

        ENG_DEBUG("Switched scene to \"" << name << "\"");
    }

    Scene* CurrentScene() { return mCurrentScene; }

    void UpdateScene(double dt) {
        if(mCurrentScene != nullptr && mCurrentScene->GetState() && mCurrentScene->mSceneStarted) mCurrentScene->Update(dt);
    }

    void LateUpdateScene(double dt) {
        if(mCurrentScene != nullptr && mCurrentScene->GetState() && mCurrentScene->mSceneStarted) mCurrentScene->LateUpdate(dt);
    }

    void FixedUpdateScene(double dt) {
        if(mCurrentScene != nullptr && mCurrentScene->GetState() && mCurrentScene->mSceneStarted) mCurrentScene->FixedUpdate(dt);
    }

    void WindowSizeScene(int w, int h) {
        if(mCurrentScene != nullptr && mCurrentScene->GetState() && mCurrentScene->mSceneStarted) mCurrentScene->WindowSize(w, h);
    }

    void MousePositionScene(double x, double y) {
        if(mCurrentScene != nullptr && mCurrentScene->GetState() && mCurrentScene->mSceneStarted) mCurrentScene->MousePosition(x, y);
    }

} gSceneHandler;

uint32_t Scene::gScenesCount = 0;
std::vector<Scene*> Scene::gpScenes = std::vector<Scene*>();

#endif