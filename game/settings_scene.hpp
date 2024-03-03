#pragma once
#ifndef __SETTINGS_SCENE_

#include "../engine/simple_text.hpp"
#include "../engine/ui.hpp"
#include "../engine/scene.hpp"

class SettingsScene : public Scene {
private:
    TextRenderer mDevText;
    Button mBackBtn;

public:
    SettingsScene() {
        SetSceneName("SettingsScene");
        gSceneHandler.AddScene(this);
    }

    virtual void Start() override {
        mDevText.Setup();
        mDevText.mPointSize = 2.0f;

        mBackBtn.Setup();
        mBackBtn.SetBGColor(RVec(0.4f));
        mBackBtn.SetButton(RVec(0.0f, -0.8f), RVec(0.2f, 0.1f));
        mBackBtn.SetText("\x1b[0f0Back");
    }

    virtual void Update(double) override {
        mDevText.SetText("\x1b[f00Setting must be\ndeveloped to use them");
        mDevText.UpdateTextVertices(RVec(-0.7f, 0.0f));
        mDevText.Render();

        mBackBtn.GetText()->mPointSize = TextRenderer::mHeight / 300;

        mBackBtn.RebindData();
        mBackBtn.Render();
    }

    virtual void LateUpdate(double) override {
        if(mBackBtn.LeftClicked() || mBackBtn.RightClicked()) {
            gSceneHandler.SetSceneByName("TitleScene");
        }
    }
    
} gSettingsScene;

#endif