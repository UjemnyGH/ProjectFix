#pragma once
#ifndef __TITLE_SCENE_

#include "../engine/simple_text.hpp"
#include "../engine/ui.hpp"
#include "../engine/scene.hpp"
#include "settings_scene.hpp"

class TitleScene : public Scene {
private:   
    TextRenderer mTitleText;
    TextRenderer mDeveloperNote;
    Button mNewGameBtn;
    Button mLoadGameBtn;
    Button mSettingsBtn;
    Button mQuitBtn;

public:
    TitleScene() {
        SetSceneName("TitleScene");
        gSceneHandler.AddScene(this);
    }

    virtual void Start() override {
        mTitleText.Setup();
        mTitleText.mPointSize = 8.0f;

        mDeveloperNote.Setup();

        mNewGameBtn.Setup();
        mNewGameBtn.SetButton(RVec(-0.6f, 0.25f), RVec(0.3f, 0.1f));
        mNewGameBtn.SetBGColor(RVec(0.4f, 0.4f, 0.4f, 0.33f));
        mNewGameBtn.SetText("\x1b[00fNew Game");

        mLoadGameBtn.Setup();
        mLoadGameBtn.SetButton(RVec(-0.6f, 0.0f), RVec(0.3f, 0.1f));
        mLoadGameBtn.SetBGColor(RVec(0.4f, 0.4f, 0.4f, 0.33f));
        mLoadGameBtn.SetText("\x1b[00fLoad Game");

        mSettingsBtn.Setup();
        mSettingsBtn.SetButton(RVec(-0.6f, -0.25f), RVec(0.3f, 0.1f));
        mSettingsBtn.SetBGColor(RVec(0.4f, 0.4f, 0.4f, 0.33f));
        mSettingsBtn.SetText("\x1b[0f0Settings");

        mQuitBtn.Setup();
        mQuitBtn.SetButton(RVec(-0.6f, -0.65f), RVec(0.3f, 0.1f));
        mQuitBtn.SetBGColor(RVec(0.4f, 0.4f, 0.4f, 0.33f));
        mQuitBtn.SetText("\x1b[f00Quit");
    }

    virtual void Update(double) override {
        mTitleText.SetText("\x1b[22fProject Fix");
        mTitleText.UpdateTextVertices(RVec(-0.9f, 0.85f));
        mTitleText.Render();

        mDeveloperNote.SetText("\x1b[fffv0.0.1\nDeveloped by Piotr Plombon");
        mDeveloperNote.UpdateTextVertices(RVec(-0.9f, -0.85f));
        mDeveloperNote.Render();

        mNewGameBtn.GetText()->mPointSize = TextRenderer::mHeight / 300;
        mLoadGameBtn.GetText()->mPointSize = TextRenderer::mHeight / 300;
        mSettingsBtn.GetText()->mPointSize = TextRenderer::mHeight / 300;
        mQuitBtn.GetText()->mPointSize = TextRenderer::mHeight / 300;

        mNewGameBtn.RebindData();
        mNewGameBtn.Render();

        mLoadGameBtn.RebindData();
        mLoadGameBtn.Render();

        mSettingsBtn.RebindData();
        mSettingsBtn.Render();

        mQuitBtn.RebindData();
        mQuitBtn.Render();
    }

    virtual void LateUpdate(double) override {
        if(mQuitBtn.RightClicked() || mQuitBtn.LeftClicked()) {
            exit(0);
        }

        if(mSettingsBtn.RightClicked() || mSettingsBtn.LeftClicked()) {
            gSceneHandler.SetSceneByName("SettingsScene");
        }

        if(mNewGameBtn.RightClicked() || mNewGameBtn.LeftClicked()) {
            gSceneHandler.SetSceneByName("TestGameScene");
        }
    }
    
} gTitleScene;

#endif