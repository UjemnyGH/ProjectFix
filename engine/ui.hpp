#pragma once
#ifndef __UI_
#define __UI_

#include "simple_text.hpp"
#include "engine_math.hpp"
#include "render_data_maker.hpp"
#include "window.hpp"
//#include <GLFW/glfw3.h>

const char* gUIVertexShader = 
"#version 450 core\n"
"layout(location = 0) in vec4 i_Position;\n"
"layout(location = 1) in vec2 i_TextureCoordinates;\n"
"layout(location = 2) in vec4 i_Color;\n"
"out vec2 v_TextureCoordinates;\n"
"out vec4 v_Color;\n"
"void main() {\n"
"   gl_Position = i_Position;\n"
"   v_TextureCoordinates = i_TextureCoordinates;\n"
"   v_Color = i_Color;\n"
"}\n";

const char* gUIFragmentShader = 
"#version 450 core\n"
"uniform sampler2DArray u_TexturesArray;\n"
"uniform float u_UsedTexture;\n"
"in vec2 v_TextureCoordinates;\n"
"in vec4 v_Color;\n"
"out vec4 o_Color;\n"
"void main() {\n"
"   o_Color = texture(u_TexturesArray, vec3(v_TextureCoordinates.xy, u_UsedTexture)) * v_Color;\n"
"}\n";

class UIComponent : public EObject {
public:
    static uint32_t gUIComponentsCount;

    static double mMouseX;
    static double mMouseY;
    static bool mMouseRClick;
    static bool mMouseLClick;

    static std::vector<UIComponent*> gpUIComponents;

    UIComponent() : EObject("UIComponent" + std::to_string(UIComponent::gUIComponentsCount), "UICOMPONENT_CLASS") { UIComponent::gUIComponentsCount++; UIComponent::gpUIComponents.push_back(this); mCommonType = EObjectCommonType_UIComponent; }

    static void PassCursorPos(double x, double y) {
        mMouseX = x;
        mMouseY = y;
    }

    static void CheckMouseClick(GLFWwindow* wnd) {
        mMouseLClick = glfwGetMouseButton(wnd, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS;
        mMouseRClick = glfwGetMouseButton(wnd, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS;
    }

    /**
     * @brief Make default color textures
     * 
     * Indexes:
     *  0 - no action
     *  1 - hover
     *  2 - click
     *  3 - disabled
     * 
     * @param width 
     * @param height 
     * @param color 
     * @return std::vector<uint8_t>
     */
    static std::vector<uint8_t> MakeDefaultTextures(int width, int height, RVec color = RVec(1.0f)) {
        std::vector<uint8_t> default_textures;
        
        for(int i = 0; i < width * height; i++) {
            default_textures.push_back((uint8_t)(color.x * 255.0f));
            default_textures.push_back((uint8_t)(color.y * 255.0f));
            default_textures.push_back((uint8_t)(color.z * 255.0f));
            default_textures.push_back((uint8_t)(color.w * 255.0f));
        }

        for(int i = 0; i < width * height; i++) {
            default_textures.push_back((uint8_t)(color.x * 255.0f) - 64);
            default_textures.push_back((uint8_t)(color.y * 255.0f) - 64);
            default_textures.push_back((uint8_t)(color.z * 255.0f) - 64);
            default_textures.push_back((uint8_t)(color.w * 255.0f) - 64);
        }

        for(int i = 0; i < width * height; i++) {
            default_textures.push_back((uint8_t)(color.x * 255.0f) - 96);
            default_textures.push_back((uint8_t)(color.y * 255.0f) - 96);
            default_textures.push_back((uint8_t)(color.z * 255.0f) - 96);
            default_textures.push_back((uint8_t)(color.w * 255.0f) - 96);
        }

        for(int i = 0; i < width * height; i++) {
            default_textures.push_back((uint8_t)(color.x * 255.0f) - 128);
            default_textures.push_back((uint8_t)(color.y * 255.0f) - 128);
            default_textures.push_back((uint8_t)(color.z * 255.0f) - 128);
            default_textures.push_back((uint8_t)(color.w * 255.0f) - 128);
        }

        return default_textures;
    }

    /**
     * @brief Make default color texture, 1 to be exact
     * 
     * @param width 
     * @param height 
     * @param color 
     * @return std::vector<uint8_t>
     */
    static std::vector<uint8_t> MakeDefaultTexture(int width, int height, RVec color = RVec(1.0f)) {
        std::vector<uint8_t> default_textures;
        
        for(int i = 0; i < width * height; i++) {
            default_textures.push_back((uint8_t)(color.x * 255.0f));
            default_textures.push_back((uint8_t)(color.y * 255.0f));
            default_textures.push_back((uint8_t)(color.z * 255.0f));
            default_textures.push_back((uint8_t)(color.w * 255.0f));
        }

        return default_textures;
    }
};

double UIComponent::mMouseX = 0.0;
double UIComponent::mMouseY = 0.0;
bool UIComponent::mMouseLClick = false;
bool UIComponent::mMouseRClick = false;
uint32_t UIComponent::gUIComponentsCount = 0;
std::vector<UIComponent*> UIComponent::gpUIComponents = std::vector<UIComponent*>();

/*class EditBox : public UIComponent {
private:
    TextRenderer mText;
    std::string mDrawnText;
    RVec mTextPosition;
    
public:
    void Setup() {
        mText.Setup();
    }

    void SetTextPosition(RVec pos) {
        mTextPosition = pos;
    }

    void Render() {
        mDrawnText += (char)Window::GetChar();
        mText.SetText(mDrawnText);
        mText.UpdateTextVertices(mTextPosition);
        mText.Render();
    }

    TextRenderer* GetTextRenderer() { return &mText; }
};*/

class Button : public UIComponent {
private:
    static uint32_t gButtonsCount;

    RVec mPositionMax;
    RVec mPositionMin;
    TextRenderer mText;

    MeshData mButtonMesh;
    RVec mTextPosition;

    ShaderProgram mProgram;
    VertexArray mArray;
    VertexBuffer mVerticesBuff;
    VertexBuffer mColorBuff;
    VertexBuffer mTextureCoordinatesBuff;
    TextureArrayBuffer mTextureArrayBuff;
    Shader mVertexShader;
    Shader mFragmentShader;
    std::string mButtonText;

public:
    static std::vector<Button*> gpButtons;

    bool mDisabled = false;

    Button() {
        mName = "Button" + std::to_string(gButtonsCount);
        mType = "BUTTON_CLASS";
        mCommonType = EObjectCommonType_Button; 
        Button::gpButtons.push_back(this);
        Button::gButtonsCount++;

        mButtonMesh.mVertices = {
            0.0f, 0.0f, 
            0.0f, 0.0f, 
            0.0f, 0.0f, 
            0.0f, 0.0f, 
            0.0f, 0.0f, 
            0.0f, 0.0f
        };

        mButtonMesh.mColors = {
            1.0f, 1.0f, 1.0f, 1.0f, 
            1.0f, 1.0f, 1.0f, 1.0f, 
            1.0f, 1.0f, 1.0f, 1.0f, 
            1.0f, 1.0f, 1.0f, 1.0f, 
            1.0f, 1.0f, 1.0f, 1.0f, 
            1.0f, 1.0f, 1.0f, 1.0f
        };

        mButtonMesh.mTextureCoords = {
            0.0f, 0.0f, 
            1.0f, 0.0f, 
            0.0f, 1.0f, 
            1.0f, 0.0f, 
            0.0f, 1.0f, 
            1.0f, 1.0f
        };
    }

    bool IsHovered() {
        if(mDisabled) return false;
        return (mMouseX / TextRenderer::mWidth) * 2.0f - 1.0f >= mPositionMin.x && (mMouseX / TextRenderer::mWidth) * 2.0f - 1.0f <= mPositionMax.x && -(mMouseY / TextRenderer::mHeight) * 2.0f + 1.0f >= mPositionMin.y && -(mMouseY / TextRenderer::mHeight) * 2.0f + 1.0f <= mPositionMax.y;
    }

    bool LeftClicked() {
        return mMouseLClick && IsHovered();
    }

    bool RightClicked() {
        return mMouseRClick && IsHovered();
    }

    void SetBGColor(RVec color) {
        mButtonMesh.mColors.clear();

        for(size_t i = 0; i < 6; i++) {
            mButtonMesh.mColors.push_back(color.x);
            mButtonMesh.mColors.push_back(color.y);
            mButtonMesh.mColors.push_back(color.z);
            mButtonMesh.mColors.push_back(color.w);
        }

        mArray.Bind();

        mColorBuff.BindData(mButtonMesh.mColors);
        
        mArray.Unbind();
    }

    void Setup() {
        mText.Setup();

        mVertexShader.MemoryShader(gUIVertexShader, GL_VERTEX_SHADER);
        mFragmentShader.MemoryShader(gUIFragmentShader, GL_FRAGMENT_SHADER);

        mProgram.Attach(mVertexShader.mID);
        mProgram.Attach(mFragmentShader.mID);
        mProgram.Link();

        mArray.Bind();

        mTextureArrayBuff.Bind();
        mTextureArrayBuff.BindData(UIComponent::MakeDefaultTextures(32, 32), 32, 32, false, 4);

        mProgram.Use();
        mProgram.SetInt1("u_TexturesArray", 0);
        mProgram.SetFloat1("u_UsedTexture", 0.0f);
        mProgram.Unuse();

        mVerticesBuff.BindData(mButtonMesh.mVertices);
        mTextureCoordinatesBuff.BindData(mButtonMesh.mTextureCoords);
        mColorBuff.BindData(mButtonMesh.mColors);
        mVerticesBuff.BindDataToPlace(0, 2);
        mTextureCoordinatesBuff.BindDataToPlace(1, 2);
        mColorBuff.BindDataToPlace(2, 4);

        mArray.Unbind();
    }

    void SetButton(RVec pos, RVec size) {
        mPositionMax = pos + size;
        mPositionMin = pos - size;

        mButtonMesh.mVertices = {
            mPositionMin.x, mPositionMin.y, 
            mPositionMax.x, mPositionMin.y, 
            mPositionMin.x, mPositionMax.y, 
            mPositionMax.x, mPositionMin.y, 
            mPositionMin.x, mPositionMax.y, 
            mPositionMax.x, mPositionMax.y
        };

        mTextPosition = pos;

        mArray.Bind();

        mVerticesBuff.BindData(mButtonMesh.mVertices);
        
        mArray.Unbind();
    }

    void SetText(std::string text) {
        mButtonText = text;
    }

    void AddText(std::string text) {
        mButtonText += text;
    }

    void RebindData() {        
        mArray.Bind();

        mVerticesBuff.BindData(mButtonMesh.mVertices);
        mTextureCoordinatesBuff.BindData(mButtonMesh.mTextureCoords);
        mColorBuff.BindData(mButtonMesh.mColors);
        
        mArray.Unbind();
    }

    void Render() {
        if(mDisabled) {
            mProgram.Use();
            mProgram.SetFloat1("u_UsedTexture", 3.0f);
            mProgram.Unuse();
        }
        else if(LeftClicked() || RightClicked()) {
            mProgram.Use();
            mProgram.SetFloat1("u_UsedTexture", 2.0f);
            mProgram.Unuse();
        }
        else if(IsHovered()) {
            mProgram.Use();
            mProgram.SetFloat1("u_UsedTexture", 1.0f);
            mProgram.Unuse();
        }
        else {
            mProgram.Use();
            mProgram.SetFloat1("u_UsedTexture", 0.0f);
            mProgram.Unuse();
        }

        mProgram.Use();
        mArray.Bind();
        mTextureArrayBuff.Bind();

        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glBlendFunc(1, 0);
        glDrawArrays(GL_TRIANGLES, 0, mButtonMesh.mVertices.size() / 2);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        mProgram.SetFloat1("u_UsedTexture", 0.0f);

        mProgram.Unuse();
        mArray.Unbind();

        mText.SetText(mButtonText);
        mText.UpdateTextVertices(mTextPosition - RVec((mText.GetText().length() * 16.0 * (mText.mPointSize) * (1.0 / (double)TextRenderer::mWidth)) / 2.0, -16.0 * (2.0 / (double)TextRenderer::mWidth)));
        mText.Render();
    }

    TextRenderer* GetText() { return &mText; }

    MeshData* GetButtonMesh() { return &mButtonMesh; }
    RVec* GetTextPosition() { return &mTextPosition; }

    ShaderProgram* GetShaderProgram() { return &mProgram; }
    VertexArray* GetVertexArray() { return &mArray; }
    VertexBuffer* GetVerticesBuffer() { return &mVerticesBuff; }
    VertexBuffer* GetColorBuffer() { return &mColorBuff; }
    VertexBuffer* GetTextureCoordinatesBuffer() { return &mTextureCoordinatesBuff; }
    TextureArrayBuffer* GetTextureArrayBuffer() { return &mTextureArrayBuff; }
};

std::vector<Button*> Button::gpButtons = std::vector<Button*>();
uint32_t Button::gButtonsCount = 0;

/*template<typename T>
class Slider : public UIComponent {
private:
    T mSliderValueMin = (T)0;
    T mSliderValueMax = (T)1;
    T mCurrentValue;
    Button mSliderButton;

    RVec mSlideMax;
    RVec mSlideMin;

    Shader mVertexShader, mFragmentShader;
    ShaderProgram mProgram;
    VertexArray mArray;
    VertexBuffer mVerticesBuffer, mColorBuffer, mTextureCoordsBuffer;
    TextureArrayBuffer mTextureArray;

    MeshData mSliderMesh;

public:
    Slider<T>() {
        mSliderMesh.mVertices = {
            0.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 0.0f
        };

        mSliderMesh.mColors = {
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f
        };

        mSliderMesh.mTextureCoords = {
            0.0f, 0.0f, 
            1.0f, 0.0f, 
            0.0f, 1.0f, 
            1.0f, 0.0f, 
            0.0f, 1.0f, 
            1.0f, 1.0f
        }
    }

    void Setup() {
        mSliderButton.Setup();

        mVertexShader.MemoryShader(gUIVertexShader, GL_VERTEX_SHADER);
        mFragmentShader.MemoryShader(gUIFragmentShader, GL_FRAGMENT_SHADER);

        mProgram.Attach(mVertexShader.mID);
        mProgram.Attach(mFragmentShader.mID);
        mProgram.Link();

        mArray.Bind();

        mTextureArray.Bind();
        mTextureArray.BindData(UIComponent::MakeDefaultTexture(32, 32), 32, 32);

        mProgram.Use();
        mProgram.SetInt1("u_TexturesArray", 0);
        mProgram.SetFloat1("u_UsedTexture", 0.0f);
        mProgram.Unuse();

        mVerticesBuffer.BindData(mSliderMesh.mVertices);
        mVerticesBuffer.BindDataToPlace(0, 2);

        mColorBuffer.BindData(mSliderMesh.mColors);
        mColorBuffer.BindDataToPlace(1, 4);

        mTextureCoordsBuffer.BindData(mSliderMesh.mTextureCoords);
        mTextureCoordsBuffer.BindDataToPlace(2, 2);

        mArray.Unbind();
    }

    void SetSlider(RVec pos, RVec size) {
        mSlideMin = pos - size;
        mSlideMax = pos + size;

        mSliderMesh.mVertices = {
            mSlideMin.x, mSlideMin.y, 
            mSlideMax.x, mSlideMin.y, 
            mSlideMin.x, mSlideMax.y, 
            mSlideMax.x, mSlideMin.y, 
            mSlideMin.x, mSlideMax.y, 
            mSlideMax.x, mSlideMax.y
        };

        mArray.Bind();

        mVerticesBuffer.BindData(mSliderMesh.mVertices);

        mArray.Unbind();
    }

    void Render() {
        mArray.Bind();
        mProgram.Use();

        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glBlendFunc(1, 0);
        glDrawArrays(GL_TRIANGLES, 0, mButtonMesh.mVertices.size() / 2);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        mProgram.SetFloat1("u_UsedTexture", 0.0f);

        mArray.Unbind();
        mProgram.Unuse();

        mSliderButton.Render();
    }
};*/

#endif