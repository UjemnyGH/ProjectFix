#pragma once
#ifndef __LIGHTS_
#define __LIGHTS_

#include "renderer.hpp"

struct Material {
    RVec mAmbient;
    //RVec mDiffuse;
    //RVec mSpecular;
    float mShininess;

    static void Pass(Renderer* pRenderer, Material mat) {
        pRenderer->GetShaderProgramPtr()->Use();

        pRenderer->GetShaderProgramPtr()->SetFloat3("u_Material.mAmbient", mat.mAmbient.x, mat.mAmbient.y, mat.mAmbient.z);
        //pRenderer->GetShaderProgramPtr()->SetFloat3("u_Material.mDiffuse", mat.mDiffuse.x, mat.mDiffuse.y, mat.mDiffuse.z);
        //pRenderer->GetShaderProgramPtr()->SetFloat3("u_Material.mSpecular", mat.mSpecular.x, mat.mSpecular.y, mat.mSpecular.z);
        pRenderer->GetShaderProgramPtr()->SetFloat1("u_Material.mShininess", mat.mShininess);

        pRenderer->GetShaderProgramPtr()->Unuse();
    }

    static void Pass(ShaderProgram* pShaderProgram, Material mat) {
        pShaderProgram->Use();

        pShaderProgram->SetFloat3("u_Material.mAmbient", mat.mAmbient.x, mat.mAmbient.y, mat.mAmbient.z);
        //pShaderProgram->SetFloat3("u_Material.mDiffuse", mat.mDiffuse.x, mat.mDiffuse.y, mat.mDiffuse.z);
        //pShaderProgram->SetFloat3("u_Material.mSpecular", mat.mSpecular.x, mat.mSpecular.y, mat.mSpecular.z);
        pShaderProgram->SetFloat1("u_Material.mShininess", mat.mShininess);

        pShaderProgram->Unuse();
    }
};

struct DirLight {
    RVec mDirection;
    RVec mAmbient;
    RVec mDiffuse;
    RVec mSpecular;

    static void Pass(Renderer* pRenderer, DirLight light) {
        pRenderer->GetShaderProgramPtr()->Use();

        pRenderer->GetShaderProgramPtr()->SetFloat3("u_DirLight.mAmbient", light.mAmbient.x, light.mAmbient.y, light.mAmbient.z);
        pRenderer->GetShaderProgramPtr()->SetFloat3("u_DirLight.mDiffuse", light.mDiffuse.x, light.mDiffuse.y, light.mDiffuse.z);
        pRenderer->GetShaderProgramPtr()->SetFloat3("u_DirLight.mSpecular", light.mSpecular.x, light.mSpecular.y, light.mSpecular.z);
        pRenderer->GetShaderProgramPtr()->SetFloat3("u_DirLight.mDirection", light.mDirection.x, light.mDirection.y, light.mDirection.z);

        pRenderer->GetShaderProgramPtr()->Unuse();
    }

    static void Pass(ShaderProgram* pShaderProgram, DirLight light) {
        pShaderProgram->Use();

        pShaderProgram->SetFloat3("u_DirLight.mAmbient", light.mAmbient.x, light.mAmbient.y, light.mAmbient.z);
        pShaderProgram->SetFloat3("u_DirLight.mDiffuse", light.mDiffuse.x, light.mDiffuse.y, light.mDiffuse.z);
        pShaderProgram->SetFloat3("u_DirLight.mSpecular", light.mSpecular.x, light.mSpecular.y, light.mSpecular.z);
        pShaderProgram->SetFloat3("u_DirLight.mDirection", light.mDirection.x, light.mDirection.y, light.mDirection.z);

        pShaderProgram->Unuse();
    }
};

struct PointLight {
    RVec mPosition;
    RVec mAmbient;
    RVec mDiffuse;
    RVec mSpecular;
    float mContrast;
    float mLinear;
    float mQuadratic;

    static void Pass(Renderer* pRenderer, PointLight* lights, uint8_t lights_count) {
        pRenderer->GetShaderProgramPtr()->Use();

        pRenderer->GetShaderProgramPtr()->SetFloat1("u_PointLightsCount", (float)lights_count);

        for(int i = 0; i < (int)lights_count; i++) {
            pRenderer->GetShaderProgramPtr()->SetFloat3(std::string("u_PointLights[") + std::to_string(i) + std::string("].mAmbient"), lights[i].mAmbient.x, lights[i].mAmbient.y, lights[i].mAmbient.z);
            pRenderer->GetShaderProgramPtr()->SetFloat3(std::string("u_PointLights[") + std::to_string(i) + std::string("].mDiffuse"), lights[i].mDiffuse.x, lights[i].mDiffuse.y, lights[i].mDiffuse.z);
            pRenderer->GetShaderProgramPtr()->SetFloat3(std::string("u_PointLights[") + std::to_string(i) + std::string("].mSpecular"), lights[i].mSpecular.x, lights[i].mSpecular.y, lights[i].mSpecular.z);
            pRenderer->GetShaderProgramPtr()->SetFloat3(std::string("u_PointLights[") + std::to_string(i) + std::string("].mPosition"), lights[i].mPosition.x, lights[i].mPosition.y, lights[i].mPosition.z);
            pRenderer->GetShaderProgramPtr()->SetFloat1(std::string("u_PointLights[") + std::to_string(i) + std::string("].mContrast"), lights[i].mContrast);
            pRenderer->GetShaderProgramPtr()->SetFloat1(std::string("u_PointLights[") + std::to_string(i) + std::string("].mLinear"), lights[i].mLinear);
            pRenderer->GetShaderProgramPtr()->SetFloat1(std::string("u_PointLights[") + std::to_string(i) + std::string("].mQuadratic"), lights[i].mQuadratic);
        }

        pRenderer->GetShaderProgramPtr()->Unuse();
    }

    static void Pass(ShaderProgram* pShaderProgram, PointLight* lights, uint8_t lights_count) {
        pShaderProgram->Use();

        pShaderProgram->SetFloat1("u_PointLightsCount", (float)lights_count);

        for(int i = 0; i < (int)lights_count; i++) {
            pShaderProgram->SetFloat3(std::string("u_PointLights[") + std::to_string(i) + std::string("].mAmbient"), lights[i].mAmbient.x, lights[i].mAmbient.y, lights[i].mAmbient.z);
            pShaderProgram->SetFloat3(std::string("u_PointLights[") + std::to_string(i) + std::string("].mDiffuse"), lights[i].mDiffuse.x, lights[i].mDiffuse.y, lights[i].mDiffuse.z);
            pShaderProgram->SetFloat3(std::string("u_PointLights[") + std::to_string(i) + std::string("].mSpecular"), lights[i].mSpecular.x, lights[i].mSpecular.y, lights[i].mSpecular.z);
            pShaderProgram->SetFloat3(std::string("u_PointLights[") + std::to_string(i) + std::string("].mPosition"), lights[i].mPosition.x, lights[i].mPosition.y, lights[i].mPosition.z);
            pShaderProgram->SetFloat1(std::string("u_PointLights[") + std::to_string(i) + std::string("].mContrast"), lights[i].mContrast);
            pShaderProgram->SetFloat1(std::string("u_PointLights[") + std::to_string(i) + std::string("].mLinear"), lights[i].mLinear);
            pShaderProgram->SetFloat1(std::string("u_PointLights[") + std::to_string(i) + std::string("].mQuadratic"), lights[i].mQuadratic);
        }

        pShaderProgram->Unuse();
    }
};

struct SpotLight {
    RVec mPosition;
    RVec mDirection;
    float mInnerCutoff;
    float mOuterCutoff;
    RVec mAmbient;
    RVec mDiffuse;
    RVec mSpecular;

    static void Pass(Renderer* pRenderer, SpotLight* lights, uint8_t lights_count) {
        pRenderer->GetShaderProgramPtr()->Use();

        pRenderer->GetShaderProgramPtr()->SetFloat1("u_SpotLightsCount", (float)lights_count);

        for(int i = 0; i < (int)lights_count; i++) {
            pRenderer->GetShaderProgramPtr()->SetFloat3(std::string("u_SpotLights[") + std::to_string(i) + std::string("].mAmbient"), lights[i].mAmbient.x, lights[i].mAmbient.y, lights[i].mAmbient.z);
            pRenderer->GetShaderProgramPtr()->SetFloat3(std::string("u_SpotLights[") + std::to_string(i) + std::string("].mDiffuse"), lights[i].mDiffuse.x, lights[i].mDiffuse.y, lights[i].mDiffuse.z);
            pRenderer->GetShaderProgramPtr()->SetFloat3(std::string("u_SpotLights[") + std::to_string(i) + std::string("].mSpecular"), lights[i].mSpecular.x, lights[i].mSpecular.y, lights[i].mSpecular.z);
            pRenderer->GetShaderProgramPtr()->SetFloat3(std::string("u_SpotLights[") + std::to_string(i) + std::string("].mPosition"), lights[i].mPosition.x, lights[i].mPosition.y, lights[i].mPosition.z);
            pRenderer->GetShaderProgramPtr()->SetFloat3(std::string("u_SpotLights[") + std::to_string(i) + std::string("].mDirection"), lights[i].mDirection.x, lights[i].mDirection.y, lights[i].mDirection.z);
            pRenderer->GetShaderProgramPtr()->SetFloat1(std::string("u_SpotLights[") + std::to_string(i) + std::string("].mInnerCutoff"), lights[i].mInnerCutoff);
            pRenderer->GetShaderProgramPtr()->SetFloat1(std::string("u_SpotLights[") + std::to_string(i) + std::string("].mOuterCutoff"), lights[i].mOuterCutoff);
        }

        pRenderer->GetShaderProgramPtr()->Unuse();
    }

    static void Pass(ShaderProgram* pShaderProgram, SpotLight* lights, uint8_t lights_count) {
        pShaderProgram->Use();

        pShaderProgram->SetFloat1("u_SpotLightsCount", (float)lights_count);

        for(int i = 0; i < (int)lights_count; i++) {
            pShaderProgram->SetFloat3(std::string("u_SpotLights[") + std::to_string(i) + std::string("].mAmbient"), lights[i].mAmbient.x, lights[i].mAmbient.y, lights[i].mAmbient.z);
            pShaderProgram->SetFloat3(std::string("u_SpotLights[") + std::to_string(i) + std::string("].mDiffuse"), lights[i].mDiffuse.x, lights[i].mDiffuse.y, lights[i].mDiffuse.z);
            pShaderProgram->SetFloat3(std::string("u_SpotLights[") + std::to_string(i) + std::string("].mSpecular"), lights[i].mSpecular.x, lights[i].mSpecular.y, lights[i].mSpecular.z);
            pShaderProgram->SetFloat3(std::string("u_SpotLights[") + std::to_string(i) + std::string("].mPosition"), lights[i].mPosition.x, lights[i].mPosition.y, lights[i].mPosition.z);
            pShaderProgram->SetFloat3(std::string("u_SpotLights[") + std::to_string(i) + std::string("].mDirection"), lights[i].mDirection.x, lights[i].mDirection.y, lights[i].mDirection.z);
            pShaderProgram->SetFloat1(std::string("u_SpotLights[") + std::to_string(i) + std::string("].mInnerCutoff"), lights[i].mInnerCutoff);
            pShaderProgram->SetFloat1(std::string("u_SpotLights[") + std::to_string(i) + std::string("].mOuterCutoff"), lights[i].mOuterCutoff);
        }

        pShaderProgram->Unuse();
    }
};


#endif