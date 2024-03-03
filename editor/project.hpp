#pragma once
#ifndef __PROJECT_
#define __PROJECT_

#include "../engine/renderer.hpp"
#include "../engine/camera.hpp"
#include "../engine/actor.hpp"
#include "../engine/data_files.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"
#include <Windows.h>

class SearchFile {
public:
    std::vector<std::string> mFilenames;
    std::vector<std::string> mDirectories;

    void operator()(std::string const& rootPath, std::string const& pattern) {
        WIN32_FIND_DATAA wfd;
        HANDLE hf;
        std::vector<std::string> dir;

        std::string find_temp = rootPath + "\\*";

        hf = FindFirstFileA(find_temp.c_str(), &wfd);

        while(hf != INVALID_HANDLE_VALUE) {
            if(wfd.cFileName[0] != '.' && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                ENG_DEBUG("Found dir: " << rootPath + "\\" + wfd.cFileName);
                dir.push_back(rootPath + "\\" + wfd.cFileName);
                mDirectories.push_back(rootPath + "\\" + wfd.cFileName);
            }

            if(!FindNextFileA(hf, &wfd)) {
                ENG_DEBUG("End dir");
                FindClose(hf);
                hf = INVALID_HANDLE_VALUE;
            }
        }

        find_temp = rootPath + "\\" + pattern;

        hf = FindFirstFileA(find_temp.c_str(), &wfd);

        while(hf != INVALID_HANDLE_VALUE) {
            if(wfd.cFileName[0] != '.' && !(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                ENG_DEBUG("Found file: " << rootPath + "\\" + wfd.cFileName);
                mFilenames.push_back(rootPath + "\\" + wfd.cFileName);
            }

            if(!FindNextFileA(hf, &wfd)) {
                ENG_DEBUG("End file");
                FindClose(hf);
                hf = INVALID_HANDLE_VALUE;
            }
        }

        for(std::vector<std::string>::iterator it = dir.begin(); it != dir.end(); ++it) {
            this->operator()(*it, pattern);
        }
    }
};

class EdProject {
private:
    std::string mProjectName;
    std::string mProjectFileName;
    SearchFile mSceneFiles, mPrefabFiles, mModelFiles, mTextureFiles, mLuaFiles, mOtherFiles;

    bool mShowNewPop = false;
    bool mShowSaveAsPop = false;
    bool mShowSavePop = false;
    bool mShowOpenPop = false;
    int mCurrentSelectedSearching = 0;
    int mCurrentItem = -1;
    int mObjectsCount = 0;
    uint32_t mRendererModelToLoadID = 0;

    void ShowEObject(EObject* pObj) {
        ImGui::Text("Object");
        ImGui::InputText("Name", gEObjectHandler[mCurrentItem]->GetNamePtr());
        ImGui::InputText("Tag", gEObjectHandler[mCurrentItem]->GetTagPtr());
        ImGui::InputText("Layer", gEObjectHandler[mCurrentItem]->GetLayerPtr());
        
        if(ImGui::TreeNode("Childs")) {
            for(auto & child : gEObjectHandler[mCurrentItem]->mChilds) {
                ImGui::Selectable(child->GetName().c_str());
            }

            ImGui::TreePop();
        }
    }

    void ShowActor(Actor* pAct) {
        ShowEObject((EObject*)pAct);

        ImGui::Text("Actor");

        float actor_position[3] = {
            pAct->GetTransform()->GetPosition().x,
            pAct->GetTransform()->GetPosition().y,
            pAct->GetTransform()->GetPosition().z
        };

        ImGui::DragFloat3("Position", actor_position);

        pAct->GetTransform()->SetPosition(FVec(
            actor_position[0],
            actor_position[1],
            actor_position[2]));
    }

    void ShowRenderer(Renderer* pRend) {
        ShowEObject((EObject*)pRend);

        ImGui::Text("Renderer");

        if(ImGui::BeginCombo("Models", "model_to_load")) {
            for(uint32_t i = 0; i < mModelFiles.mFilenames.size(); i++) {
                const bool is_selected = (mRendererModelToLoadID == i);
                if (ImGui::Selectable(mModelFiles.mFilenames[i].c_str(), is_selected)) mRendererModelToLoadID = i;

                if (is_selected) ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        if(ImGui::Button("Add model")) {
            pRend->AddData(MeshData::LoadOBJModelTriangulated(mModelFiles.mFilenames[mRendererModelToLoadID]));
        }

        if(ImGui::TreeNode("Renderer models")) {
            for(uint32_t i = 0; i < pRend->GetRenderData()->mOriginalData.size(); i++) {
                ImGui::Selectable(pRend->GetRenderData()->mOriginalData[i].mPath.c_str());
            }

            ImGui::TreePop();
        }
    }

public:
    DataFile mProjectFile;

    void ShowNewProjectPopup() { mShowNewPop = true; }
    void ShowSaveAsProjectPopup() { mShowSaveAsPop = true; }
    void ShowSaveProjectPopup() { mShowSavePop = true; }
    void ShowOpenProjectPopup() { mShowOpenPop = true; }

    void ImGuiWindowHandle() {
        if(ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
            int common_type = 0;
            std::string object_name;
            std::string tag_name;
            std::string layer_name;
            
            if(mCurrentItem >= 0) {
                common_type = gEObjectHandler[mCurrentItem]->GetCommonType();
                object_name = gEObjectHandler[mCurrentItem]->GetName();
                tag_name = gEObjectHandler[mCurrentItem]->GetTag();
                layer_name = gEObjectHandler[mCurrentItem]->GetLayer();

                if(common_type == EObjectCommonType_Object) {
                    ShowEObject(gEObjectHandler[mCurrentItem]);
                }
                else if(common_type == EObjectCommonType_Actor) {
                    ShowActor(reinterpret_cast<Actor*>(gEObjectHandler[mCurrentItem]));
                }
                else if(common_type == EObjectCommonType_Renderer) {
                    ShowRenderer(reinterpret_cast<Renderer*>(gEObjectHandler[mCurrentItem]));
                }

                /*switch(common_type) {
                    case EObjectCommonType_Object:
                        ImGui::Text("Object");
                        ImGui::InputText("Name", gEObjectHandler[mCurrentItem]->GetNamePtr());
                        ImGui::InputText("Tag", gEObjectHandler[mCurrentItem]->GetTagPtr());
                        ImGui::InputText("Layer", gEObjectHandler[mCurrentItem]->GetLayerPtr());
                        
                        if(ImGui::TreeNode("Childs")) {
                            for(auto & child : gEObjectHandler[mCurrentItem]->mChilds) {
                                ImGui::Selectable(child->GetName().c_str());
                            }

                            ImGui::TreePop();
                        }

                        break;

                    case EObjectCommonType_Actor:
                        ImGui::Text("Actor");

                        ImGui::InputText("Name", gEObjectHandler[mCurrentItem]->GetNamePtr());
                        ImGui::InputText("Tag", gEObjectHandler[mCurrentItem]->GetTagPtr());
                        ImGui::InputText("Layer", gEObjectHandler[mCurrentItem]->GetLayerPtr());

                        ImGui::DragFloat("Position X", &actor_ptr->GetTransform()->GetPosition().x);
                        ImGui::DragFloat("Position Y", &actor_ptr->GetTransform()->GetPosition().y);
                        ImGui::DragFloat("Position Z", &actor_ptr->GetTransform()->GetPosition().z);

                        if(ImGui::TreeNode("Childs")) {
                            for(auto & child : gEObjectHandler[mCurrentItem]->mChilds) {
                                ImGui::Selectable(child->GetName().c_str());
                            }

                            ImGui::TreePop();
                        }

                        break;

                    case EObjectCommonType_Camera:

                        break;

                    case EObjectCommonType_Renderer:

                        break;

                    case EObjectCommonType_Button:

                        break;

                    case EObjectCommonType_TextRenderer:

                        break;

                    case EObjectCommonType_Scene:

                        break;

                    case EObjectCommonType_UIComponent:

                        break;
                }*/
            }

            ImGui::End();
        }

        if(ImGui::Begin("Project", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
            if(ImGui::BeginMenuBar()) {
                if(ImGui::BeginMenu("Add")) {
                    if(ImGui::MenuItem("Renderer")) { gEObjectHandler(new Renderer()); }
                    if(ImGui::MenuItem("Camera")) { gEObjectHandler(new Camera()); }
                    if(ImGui::MenuItem("Object")) { gEObjectHandler(new EObject("Object" + std::to_string(mObjectsCount++), "OBJECT_CLASS")); }
                    if(ImGui::MenuItem("Actor")) { gEObjectHandler(new Actor()); }
                    if(ImGui::MenuItem("Button")) { gEObjectHandler(new Button()); }
                    if(ImGui::MenuItem("Text Renderer")) { gEObjectHandler(new TextRenderer()); }
                    if(ImGui::MenuItem("Scene")) { gEObjectHandler(new Scene()); }
                    if(ImGui::MenuItem("UI Component")) { gEObjectHandler(new UIComponent()); }

                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }

            for(uint32_t i = 0; i < gEObjectHandler.Size(); i++) {
                if(gEObjectHandler[i]->mChilds.empty()) {
                    if(ImGui::Selectable(gEObjectHandler[i]->GetName().c_str())) {
                        mCurrentItem = i;
                    }
                }
                else {
                    if(ImGui::TreeNode(gEObjectHandler[i]->GetName().c_str())) {
                        for(auto & child : gEObjectHandler[i]->mChilds) {
                            if(ImGui::Selectable(child->GetName().c_str())) {
                                
                            }
                        }

                        ImGui::TreePop();
                    }
                }
            }

            ImGui::End();
        }

        if(ImGui::Begin("File explorer", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
            if(ImGui::BeginMenuBar()) {
                if(ImGui::MenuItem("Refresh", "", false, !mProjectFileName.empty())) {
                    ENG_DEBUG("Project path: " << mProjectFileName);
                    mSceneFiles(mProjectFileName, "*.oes");
                    mPrefabFiles(mProjectFileName, "*.oef");
                    mModelFiles(mProjectFileName, "*.obj");
                    mTextureFiles(mProjectFileName, "*.bmp");
                    mLuaFiles(mProjectFileName, "*.lua");
                    mOtherFiles(mProjectFileName, "*.*");
                }

                ImGui::EndMenuBar();
            }

            const char* items[6] = {
                "-",
                "Scenes",
                "Prefabs",
                "Models",
                "Images/Textures",
                "Scripts",
            };

            ImGui::Combo("##select_search", &mCurrentSelectedSearching, items, 6);

            if(ImGui::BeginChild("##show_files_child")) {
                switch(mCurrentSelectedSearching) {
                    case 0:
                        for(auto it = mOtherFiles.mFilenames.begin(); it != mOtherFiles.mFilenames.end(); it++) {
                            ImGui::Selectable(it->c_str());
                        }

                        break;

                    case 1:
                        for(auto it = mSceneFiles.mFilenames.begin(); it != mSceneFiles.mFilenames.end(); it++) {
                            ImGui::Selectable(it->c_str());
                        }

                        break;

                    case 2:
                        for(auto it = mPrefabFiles.mFilenames.begin(); it != mPrefabFiles.mFilenames.end(); it++) {
                            ImGui::Selectable(it->c_str());
                        }

                        break;

                    case 3:
                        for(auto it = mModelFiles.mFilenames.begin(); it != mModelFiles.mFilenames.end(); it++) {
                            ImGui::Selectable(it->c_str());
                        }

                        break;

                    case 4:
                        for(auto it = mTextureFiles.mFilenames.begin(); it != mTextureFiles.mFilenames.end(); it++) {
                            ImGui::Selectable(it->c_str());
                        }

                        break;

                    case 5:
                        for(auto it = mLuaFiles.mFilenames.begin(); it != mLuaFiles.mFilenames.end(); it++) {
                            ImGui::Selectable(it->c_str());
                        }

                        break;
                }

                ImGui::EndChild();
            }

            ImGui::End();
        }
    }

    void ImGuiClassHandle() {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if(mShowNewPop) {
            ImGui::OpenPopup("New project");
        }

        if(ImGui::BeginPopupModal("New project", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            bool enter = ImGui::InputText("Project name", &mProjectName, ImGuiInputTextFlags_EnterReturnsTrue);
            bool enter2 = ImGui::InputText("Project path", &mProjectFileName, ImGuiInputTextFlags_EnterReturnsTrue);

            if((ImGui::Button("Create") || enter || enter2) && !mProjectName.empty() && !mProjectFileName.empty()) {
                if(mProjectFileName[mProjectFileName.size() - 1] != '/' && mProjectFileName[mProjectFileName.size() - 1] != '\\') {
                    mProjectFileName += "/";
                }

                mProjectFile.AddStruct("PROJECT_FILE_HEADER");
                mProjectFile.AddStruct("SCENES_FILES");
                mProjectFile.AddStruct("PREFAB_FILES");
                mProjectFile.AddValue("PROJECT_FILE_HEADER", "projectName", mProjectName);
                mProjectFile.AddValue("PROJECT_FILE_HEADER", "projectPath", mProjectFileName);
                mProjectFile.AddValue("PROJECT_FILE_HEADER", "mainProjectFile", "1");

                mProjectFile.SaveToFile(mProjectFileName + mProjectName + ".oep");

                mShowNewPop = false;

                ENG_DEBUG("Created new project: " << mProjectFileName + mProjectName + ".oep");

                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if(ImGui::Button("Cancel")) {
                mShowNewPop = false;
                ImGui::CloseCurrentPopup();
            }
            
            ImGui::EndPopup();
        }

        center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if(mShowSaveAsPop) {
            ImGui::OpenPopup("Save as project");
        }

        if(ImGui::BeginPopupModal("Save as project", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            bool enter = ImGui::InputText("Project name", &mProjectName, ImGuiInputTextFlags_EnterReturnsTrue);
            bool enter2 = ImGui::InputText("Project path", &mProjectFileName, ImGuiInputTextFlags_EnterReturnsTrue);

            if((ImGui::Button("Save as") || enter || enter2) && !mProjectName.empty() && !mProjectFileName.empty()) {
                if(mProjectFileName[mProjectFileName.size() - 1] != '/' && mProjectFileName[mProjectFileName.size() - 1] != '\\') {
                    mProjectFileName += "/";
                }

                mProjectFile.SaveToFile(mProjectFileName + mProjectName + ".oep");

                mShowSaveAsPop = false;

                ENG_DEBUG("Saved project");

                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if(ImGui::Button("Cancel")) {
                mShowSaveAsPop = false;

                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if(mShowSavePop) {
            ImGui::OpenPopup("Save project");
        }

        if(ImGui::BeginPopupModal("Save project", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            mProjectFile.SaveToFile(mProjectFileName + mProjectName + ".oep");

            mShowSavePop = false;

            ENG_DEBUG("Saved project");

            ImGui::CloseCurrentPopup();
            
            ImGui::EndPopup();
        }

        center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if(mShowOpenPop) {
            ImGui::OpenPopup("Open project");
        }

        if(ImGui::BeginPopupModal("Open project", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            bool enter = ImGui::InputText("Project name", &mProjectName, ImGuiInputTextFlags_EnterReturnsTrue);
            bool enter2 = ImGui::InputText("Project path", &mProjectFileName, ImGuiInputTextFlags_EnterReturnsTrue);

            if((ImGui::Button("Open") || enter || enter2) && !mProjectName.empty() && !mProjectFileName.empty()) {
                if(mProjectFileName[mProjectFileName.size() - 1] != '/' && mProjectFileName[mProjectFileName.size() - 1] != '\\') {
                    mProjectFileName += "/";
                }
                
                mProjectFile.LoadFromFile(mProjectFileName + mProjectName + ".oep");

                mShowOpenPop = false;

                ENG_DEBUG("Opened project: " << mProjectFileName + mProjectName + ".oep");

                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if(ImGui::Button("Cancel")) {
                mShowOpenPop = false;

                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }
};

#endif