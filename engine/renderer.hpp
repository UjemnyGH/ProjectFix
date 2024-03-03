#pragma once
#ifndef __RENDERER_
#define __RENDERER_

#include "buffers.hpp"
#include "render_data_maker.hpp"
#include "camera.hpp"

class Renderer : public EObject {
private:
    static uint32_t gRenderersCount;

    ShaderProgram mShaderProgram;
    VertexArray mVertexArray;
    std::vector<VertexBuffer> mVertexBuffers;
    std::vector<Shader*> mShadersPtr;
    TextureArrayBuffer mTextures[32];
    RenderData mRendererData;
    RMat* mProjectionMatrix = &gDefaultProjectionMatrix;
    RMat* mViewMatrix = &gDefaultViewMatrix;
    RMat mTransposeMatrix = RMat::Identity();

public:
    static std::vector<Renderer*> gpRenderers;

    Renderer() : EObject("Renderer" + std::to_string(Renderer::gRenderersCount), "RENDERER_CLASS") { Renderer::gRenderersCount++; Renderer::gpRenderers.push_back(this); mCommonType = EObjectCommonType_Renderer;  }

    virtual void ShouldStart() override {
        Setup();
    }

    virtual void ShouldUpdate() override {
        RebindBuffers();
        Render();
    }

    void Setup() {
        mVertexArray.Bind();

        for(int i = 0; i < 32; i++) {
            mTextures[i].Bind();
        }

        mVertexBuffers.resize(5);

        mVertexBuffers[0].BindData(mRendererData.mJoinedData.mVertices);
        mVertexBuffers[1].BindData(mRendererData.mJoinedData.mColors);
        mVertexBuffers[2].BindData(mRendererData.mJoinedData.mTextureCoords);
        mVertexBuffers[3].BindData(mRendererData.mJoinedData.mNormals);
        mVertexBuffers[4].BindData(mRendererData.mTextureIDs);

        mVertexBuffers[0].BindDataToPlace(0, 3);
        mVertexBuffers[1].BindDataToPlace(1, 4);
        mVertexBuffers[2].BindDataToPlace(2, 2);
        mVertexBuffers[3].BindDataToPlace(3, 3);
        mVertexBuffers[4].BindDataToPlace(4, 1);

        mVertexArray.Unbind();
    }

    void AddData(MeshData data) {
        mRendererData.AddData(data);
    }

    void SetProjectionMatrix(float fov, int width, int height) {
        *mProjectionMatrix = RMat::PerspectiveFOV(fov, width, height, 0.001f, 1000.0f);
    }

    void ForceRejoinRebindBuffers() {
        mRendererData.RejoinData();
        
        RebindBuffers();
    }

    void RebindBuffers() {
        mVertexArray.Bind();
        
        mVertexBuffers[0].BindData(mRendererData.mJoinedData.mVertices);
        mVertexBuffers[1].BindData(mRendererData.mJoinedData.mColors);
        mVertexBuffers[2].BindData(mRendererData.mJoinedData.mTextureCoords);
        mVertexBuffers[3].BindData(mRendererData.mJoinedData.mNormals);
        mVertexBuffers[4].BindData(mRendererData.mTextureIDs);

        mVertexArray.Unbind();
    }

    void AddShader(Shader* pShader) {
        mShadersPtr.push_back(pShader);
    }

    void DeleteShader(Shader* pShader) {
        for(size_t i = 0; i < mShadersPtr.size(); i++) {
            if(mShadersPtr[i] == pShader) {
                mShadersPtr.erase(mShadersPtr.begin() + i);
                break;
            }
        }
    }

    void ReattachShaders() {
        mShaderProgram.Unuse();
        mShaderProgram.Delete();
        mShaderProgram.mID = 0;

        for(auto s : mShadersPtr) {
            mShaderProgram.Attach(s->mID);
        }

        mShaderProgram.Link();

        mShaderProgram.Use();
        mShaderProgram.SetInt1Ptr("u_Textures", 32, gTextureSamplers);
        mShaderProgram.Unuse();
    }

    void Render() {
        mVertexArray.Bind();
        mShaderProgram.Use();

        ENG_DEBUG("Shader program and vertex array bound");

        for(int i = 0; i < 32; i++) {
            mTextures[i].BindUnit(i);
        }

        ENG_DEBUG("Texture units bound");

        mShaderProgram.SetFloatMat4("u_Projection", mProjectionMatrix->m);
        mShaderProgram.SetFloatMat4("u_View", mViewMatrix->m);
        mShaderProgram.SetFloatMat4("u_Transpose", mTransposeMatrix.m);

        ENG_DEBUG("Matrices set")

        glDrawArrays(GL_TRIANGLES, 0, mRendererData.mJoinedData.mVertices.size() / 3);

        ENG_DEBUG("Data drawn")

        mShaderProgram.Unuse();
        mVertexArray.Unbind();

        ENG_DEBUG("Shader program and vertex array unbound");
    }

    ShaderProgram* GetShaderProgramPtr() { return &mShaderProgram; }
    VertexArray* GetVertexArrayPtr() { return &mVertexArray; }
    VertexBuffer* GetDesiredVertexBuffer(uint32_t index) { return &mVertexBuffers[index]; }

    std::vector<Shader*>* GetShaders() { return &mShadersPtr; }
    std::vector<VertexBuffer>* GetVectorVertexBuffers() { return &mVertexBuffers; }

    VertexBuffer* GetPositionsVertexBuffer() { return GetDesiredVertexBuffer(0); }
    VertexBuffer* GetColorVertexBuffer() { return GetDesiredVertexBuffer(1); }
    VertexBuffer* GetTextureCoordsVertexBuffer() { return GetDesiredVertexBuffer(2); }
    VertexBuffer* GetTextureIDsVertexBuffer() { return GetDesiredVertexBuffer(3); }
    VertexBuffer* GetNormalsVertexBuffer() { return GetDesiredVertexBuffer(4); }

    TextureArrayBuffer* GetTextureBuffer() { return mTextures; }
    TextureArrayBuffer* GetTextureID(uint32_t id) { return &mTextures[id]; }

    RenderData* GetRenderData() { return &mRendererData; }

    RMat* GetViewMatrix() { return mViewMatrix; }
    void SetViewMatrix(RMat *mat) { mViewMatrix = mat; }

    RMat* GetProjectionMatrix() { return mProjectionMatrix; }
    void SetProjectionMatrix(RMat *mat) { mProjectionMatrix = mat; }

    RMat* GetTransposeMatrix() { return &mTransposeMatrix; }
    void SetTransposeMatrix(RMat mat) { mTransposeMatrix = mat; }
};

#if defined(DEBUG) || defined(DEBUG_DRAW)
ShaderProgram __gDebugDrawCubeShaderProgram;
VertexArray __gDebugDrawCubeVertexArray;
VertexBuffer __gDebugDrawCubeVertexBuffer;
#endif

void _debug_Init() {
    #if defined(DEBUG) || defined(DEBUG_DRAW)
    const char* vsh = "#version 450 core\nuniform mat4 u_Proj;\nuniform mat4 u_View;\nlayout(location = 0) in vec4 i_Pos;\nvoid main() {\ngl_Position = u_Proj * u_View * i_Pos;\n}\n";
    const char* fsh = "#version 450 core\nout vec4 o_Col;\nvoid main() {\no_Col = vec4(0.0, 1.0, 0.0, 1.0);\n}\n";
    Shader vs;
    Shader fs;
    vs.MemoryShader(vsh, GL_VERTEX_SHADER);
    fs.MemoryShader(fsh, GL_FRAGMENT_SHADER);

    __gDebugDrawCubeVertexArray.Bind();
    __gDebugDrawCubeShaderProgram.Use();
    __gDebugDrawCubeShaderProgram.Attach(vs.mID);
    __gDebugDrawCubeShaderProgram.Attach(fs.mID);
    __gDebugDrawCubeShaderProgram.Link();
    __gDebugDrawCubeVertexBuffer.BindDataToPlace(0, 3);

    __gDebugDrawCubeVertexArray.Unbind();
    __gDebugDrawCubeShaderProgram.Unuse();
    #endif
}

void _debug_DrawCube(RVec pos, RVec size, RMat proj = gDefaultProjectionMatrix, RMat view = gDefaultViewMatrix) {
    #if defined(DEBUG) || defined(DEBUG_DRAW)
    std::vector<float> vertices = {
        pos.x + size.x, pos.y + size.y, pos.z + size.z, 
        pos.x + size.x, pos.y - size.y, pos.z + size.z, 
        pos.x + size.x, pos.y + size.y, pos.z + size.z, 
        pos.x + size.x, pos.y + size.y, pos.z - size.z,

        pos.x + size.x, pos.y - size.y, pos.z - size.z, 
        pos.x + size.x, pos.y - size.y, pos.z + size.z, 
        pos.x + size.x, pos.y - size.y, pos.z - size.z, 
        pos.x + size.x, pos.y + size.y, pos.z - size.z, 

        pos.x - size.x, pos.y + size.y, pos.z + size.z, 
        pos.x - size.x, pos.y - size.y, pos.z + size.z, 
        pos.x - size.x, pos.y + size.y, pos.z + size.z, 
        pos.x - size.x, pos.y + size.y, pos.z - size.z, 

        pos.x - size.x, pos.y - size.y, pos.z - size.z, 
        pos.x - size.x, pos.y - size.y, pos.z + size.z, 
        pos.x - size.x, pos.y - size.y, pos.z - size.z, 
        pos.x - size.x, pos.y + size.y, pos.z - size.z, 

        pos.x - size.x, pos.y + size.y, pos.z + size.z, 
        pos.x + size.x, pos.y + size.y, pos.z + size.z, 
        pos.x - size.x, pos.y - size.y, pos.z - size.z, 
        pos.x + size.x, pos.y - size.y, pos.z - size.z, 

        pos.x - size.x, pos.y - size.y, pos.z + size.z, 
        pos.x + size.x, pos.y - size.y, pos.z + size.z, 
        pos.x - size.x, pos.y + size.y, pos.z - size.z, 
        pos.x + size.x, pos.y + size.y, pos.z - size.z, 
    };
    
    __gDebugDrawCubeShaderProgram.Use();
    __gDebugDrawCubeVertexArray.Bind();
    __gDebugDrawCubeVertexBuffer.BindData(vertices);
    __gDebugDrawCubeShaderProgram.SetFloatMat4("u_Proj", proj.m);
    __gDebugDrawCubeShaderProgram.SetFloatMat4("u_View", view.m);

    glLineWidth(2.0f);
    glDrawArrays(GL_LINES, 0, vertices.size());
    glLineWidth(1.0f);

    __gDebugDrawCubeShaderProgram.Unuse();
    __gDebugDrawCubeVertexArray.Unbind();

    #endif
}

uint32_t Renderer::gRenderersCount = 0;
std::vector<Renderer*> Renderer::gpRenderers = std::vector<Renderer*>();

#endif