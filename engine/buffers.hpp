#pragma once
#ifndef __BUFFERS_
#define __BUFFERS_

#include <glad/gl.h>
#include <vector>
#include <string>
#include <fstream>
#include "core.hpp"

struct Shader {
    uint32_t mID;

    Shader() = default;
    Shader(std::string path) {
        if(path.find(".vert") != (size_t)-1) {
            ENG_INFO("Loading vertex shader: " << path);
            FileShader(path, GL_VERTEX_SHADER);
        }
        else if(path.find(".frag") != (size_t)-1) {
            ENG_INFO("Loading fragment shader: " << path);
            FileShader(path, GL_FRAGMENT_SHADER);
        }
        else if(path.find(".geom") != (size_t)-1) {
            ENG_INFO("Loading geometry shader: " << path);
            FileShader(path, GL_GEOMETRY_SHADER);
        }
        else if(path.find(".comp") != (size_t)-1) {
            ENG_INFO("Loading compute shader: " << path);
            FileShader(path, GL_COMPUTE_SHADER);
        }
        else if(path.find(".teva") != (size_t)-1) {
            ENG_INFO("Loading tesselation evaluation shader: " << path);
            FileShader(path, GL_TESS_EVALUATION_SHADER);
        }
        else if(path.find(".teco") != (size_t)-1) {
            ENG_INFO("Loading tesselation control shader: " << path);
            FileShader(path, GL_TESS_CONTROL_SHADER);
        }
    }

    void MemoryShader(const char* src, int type) {
        mID = glCreateShader(type);
        glShaderSource(mID, 1, &src, nullptr);
        glCompileShader(mID);

        int isCompiled = 0;

        glGetShaderiv(mID, GL_COMPILE_STATUS, &isCompiled);

        if(!isCompiled) {
            int maxLength = 0;

            glGetShaderiv(mID, GL_INFO_LOG_LENGTH, &maxLength);

            std::string infoLog;
            infoLog.resize(maxLength);

            glGetShaderInfoLog(mID, maxLength, &maxLength, &infoLog[0]);

            ENG_ERR(infoLog)
        }
        else {
            ENG_DEBUG("Shader compiled succesfully")
        }
    }

    void FileShader(std::string path, int type) {
        std::ifstream f(path, std::ios_base::binary);

        f.seekg(0, std::ios_base::end);
        uint32_t length = f.tellg();
        f.seekg(0, std::ios_base::beg);

        std::string buffer;
        buffer.resize(length);

        f.read(buffer.data(), buffer.size());

        f.close();

        MemoryShader(buffer.c_str(), type);
    }

    ~Shader() {
        glDeleteShader(mID);
    }
};

struct ShaderProgram {
    uint32_t mID;
    bool mCreated = false;

    void Init() {
        if(!mCreated) {
            mID = glCreateProgram();

            mCreated = true;
        }
    }

    void Use() {
        Init();

        glUseProgram(mID);
    }

    void Link(bool show_info = true) {
        Init();

        glLinkProgram(mID);

        int isLinked = 0;

        glGetProgramiv(mID, GL_LINK_STATUS, &isLinked);

        if(!isLinked) {
            int maxLength = 0;

            glGetProgramiv(mID, GL_INFO_LOG_LENGTH, &maxLength);

            std::string infoLog;
            infoLog.resize(maxLength);
            glGetProgramInfoLog(mID, maxLength, &maxLength, &infoLog[0]);

            ENG_ERR(infoLog);
        }
        else {
            if(show_info) {
                ENG_DEBUG("Shader program linked successfully")
                ENG_INFO("Shader program linked successfully")
            }
        }
    }

    void Attach(uint32_t id) {
        Init();

        glAttachShader(mID, id);
    }

    void Detach(uint32_t id) {
        Init();

        glDetachShader(mID, id);
    }

    void Unuse() {
        glUseProgram(0);
    }

    void SetInt1(std::string location, int v0) { glUniform1i(glGetUniformLocation(mID, location.c_str()), v0); }
    void SetInt2(std::string location, int v0, int v1) { glUniform2i(glGetUniformLocation(mID, location.c_str()), v0, v1); }
    void SetInt3(std::string location, int v0, int v1, int v2) { glUniform3i(glGetUniformLocation(mID, location.c_str()), v0, v1, v2); }
    void SetInt4(std::string location, int v0, int v1, int v2, int v3) { glUniform4i(glGetUniformLocation(mID, location.c_str()), v0, v1, v2, v3); }

    void SetUInt1(std::string location, uint32_t v0) { glUniform1ui(glGetUniformLocation(mID, location.c_str()), v0); }
    void SetUInt2(std::string location, uint32_t v0, uint32_t v1) { glUniform2ui(glGetUniformLocation(mID, location.c_str()), v0, v1); }
    void SetUInt3(std::string location, uint32_t v0, uint32_t v1, uint32_t v2) { glUniform3ui(glGetUniformLocation(mID, location.c_str()), v0, v1, v2); }
    void SetUInt4(std::string location, uint32_t v0, uint32_t v1, uint32_t v2, uint32_t v3) { glUniform4ui(glGetUniformLocation(mID, location.c_str()), v0, v1, v2, v3); }

    void SetFloat1(std::string location, float v0) { glUniform1f(glGetUniformLocation(mID, location.c_str()), v0); }
    void SetFloat2(std::string location, float v0, float v1) { glUniform2f(glGetUniformLocation(mID, location.c_str()), v0, v1); }
    void SetFloat3(std::string location, float v0, float v1, float v2) { glUniform3f(glGetUniformLocation(mID, location.c_str()), v0, v1, v2); }
    void SetFloat4(std::string location, float v0, float v1, float v2, float v3) { glUniform4f(glGetUniformLocation(mID, location.c_str()), v0, v1, v2, v3); }

    void SetDouble1(std::string location, double v0) { glUniform1d(glGetUniformLocation(mID, location.c_str()), v0); }
    void SetDouble2(std::string location, double v0, double v1) { glUniform2d(glGetUniformLocation(mID, location.c_str()), v0, v1); }
    void SetDouble3(std::string location, double v0, double v1, double v2) { glUniform3d(glGetUniformLocation(mID, location.c_str()), v0, v1, v2); }
    void SetDouble4(std::string location, double v0, double v1, double v2, double v3) { glUniform4d(glGetUniformLocation(mID, location.c_str()), v0, v1, v2, v3); }

    void SetInt1Ptr(std::string location, int count, int *v0) { glUniform1iv(glGetUniformLocation(mID, location.c_str()), count, v0); }
    void SetInt2Ptr(std::string location, int count, int *v0) { glUniform2iv(glGetUniformLocation(mID, location.c_str()), count, v0); }
    void SetInt3Ptr(std::string location, int count, int *v0) { glUniform3iv(glGetUniformLocation(mID, location.c_str()), count, v0); }
    void SetInt4Ptr(std::string location, int count, int *v0) { glUniform4iv(glGetUniformLocation(mID, location.c_str()), count, v0); }

    void SetUInt1Ptr(std::string location, int count, uint32_t *v0) { glUniform1uiv(glGetUniformLocation(mID, location.c_str()), count, v0); }
    void SetUInt2Ptr(std::string location, int count, uint32_t *v0) { glUniform2uiv(glGetUniformLocation(mID, location.c_str()), count, v0); }
    void SetUInt3Ptr(std::string location, int count, uint32_t *v0) { glUniform3uiv(glGetUniformLocation(mID, location.c_str()), count, v0); }
    void SetUInt4Ptr(std::string location, int count, uint32_t *v0) { glUniform4uiv(glGetUniformLocation(mID, location.c_str()), count, v0); }

    void SetFloat1Ptr(std::string location, int count, float *v0) { glUniform1fv(glGetUniformLocation(mID, location.c_str()), count, v0); }
    void SetFloat2Ptr(std::string location, int count, float *v0) { glUniform2fv(glGetUniformLocation(mID, location.c_str()), count, v0); }
    void SetFloat3Ptr(std::string location, int count, float *v0) { glUniform3fv(glGetUniformLocation(mID, location.c_str()), count, v0); }
    void SetFloat4Ptr(std::string location, int count, float *v0) { glUniform4fv(glGetUniformLocation(mID, location.c_str()), count, v0); }

    void SetDouble1Ptr(std::string location, int count, double *v0) { glUniform1dv(glGetUniformLocation(mID, location.c_str()), count, v0); }
    void SetDouble2Ptr(std::string location, int count, double *v0) { glUniform2dv(glGetUniformLocation(mID, location.c_str()), count, v0); }
    void SetDouble3Ptr(std::string location, int count, double *v0) { glUniform3dv(glGetUniformLocation(mID, location.c_str()), count, v0); }
    void SetDouble4Ptr(std::string location, int count, double *v0) { glUniform4dv(glGetUniformLocation(mID, location.c_str()), count, v0); }

    void SetFloatMat2(std::string location, float* mat) { glUniformMatrix2fv(glGetUniformLocation(mID, location.c_str()), 1, GL_FALSE, mat); }
    void SetFloatMat3(std::string location, float* mat) { glUniformMatrix3fv(glGetUniformLocation(mID, location.c_str()), 1, GL_FALSE, mat); }
    void SetFloatMat4(std::string location, float* mat) { glUniformMatrix4fv(glGetUniformLocation(mID, location.c_str()), 1, GL_FALSE, mat); }

    void Delete() {
        if(mCreated) {
            glDeleteProgram(mID);

            mCreated = false;
            mID = 0;
        }
    }

    ~ShaderProgram() {
        Delete();
    }
};

struct VertexArray {
    uint32_t mID;
    bool mCreated = false;
    
    void Init() {
        if(!mCreated) {
            glGenVertexArrays(1, &mID);

            mCreated = true;
        }
    }

    void Bind() {
        Init();

        glBindVertexArray(mID);
    }

    void Unbind() {
        glBindVertexArray(0);
    }


    void Delete() {
        if(mCreated) {
            glDeleteVertexArrays(1, &mID);

            mCreated = false;
        }
    }
};

struct VertexBuffer {
    uint32_t mID;
    bool mCreated = false;

    void Init() {
        if(!mCreated) {
            glGenBuffers(1, &mID);

            mCreated = true;
        }
    }

    void Bind() {
        Init();

        glBindBuffer(GL_ARRAY_BUFFER, mID);
    }

    void BindData(std::vector<float> data) {
        Bind();

        glBufferData(GL_ARRAY_BUFFER, sizeof(data[0]) * data.size(), data.data(), GL_DYNAMIC_DRAW);
    }

    void BindDataToPlace(int index, int dimmensions) {
        Bind();

        glVertexAttribPointer(index, dimmensions, GL_FLOAT, 0, 0, (void*)0);
        glEnableVertexAttribArray(index);
    }

    void Delete() {
        if(mCreated) {
            glDeleteBuffers(1, &mID);

            mCreated = false;
        }
    }

    ~VertexBuffer() {
        Delete();
    }
};

struct TextureBuffer {
    uint32_t mID;
    bool mCreated = false;

    void Init() {
        if(!mCreated) {
            glGenTextures(1, &mID);

            mCreated = true;
        }
    }

    void Bind() {
        Init();

        glBindTexture(GL_TEXTURE_2D, mID);
    }

    void BindUnit(uint32_t unit_id) {
        Init();

        glBindTextureUnit(unit_id, mID);
    }

    void BindData(std::vector<uint8_t> data, uint32_t width, uint32_t height, bool pixels = false) {
        Bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        if(pixels) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
        else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 16);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
        glGenerateMipmap(GL_TEXTURE_2D);

        ENG_DEBUG("Bound texture of sizes " << width << "x" << height << (pixels == true ? " pixelized" : ""))
    }

    void Delete() {
        if(mCreated) {
            glDeleteTextures(1, &mID);

            mCreated = false;
        }
    }

    ~TextureBuffer() {
        Delete();
    }
};

struct TextureArrayBuffer {
    uint32_t mID;
    bool mCreated = false;

    void Init() {
        if(!mCreated) {
            glGenTextures(1, &mID);

            mCreated = true;
        }
    }

    void Bind() {
        Init();

        glBindTexture(GL_TEXTURE_2D_ARRAY, mID);
    }

    void BindUnit(uint32_t unit_id) {
        Init();

        glBindTextureUnit(unit_id, mID);
    }

    void BindData(std::vector<uint8_t> data, uint32_t width, uint32_t height, bool pixels = false, uint32_t layerCount = 1) {
        Bind();

        glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width, height, layerCount);
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, layerCount, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 16);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        if(pixels) {
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
        else {
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }

        ENG_DEBUG("Bound texture of sizes " << width << "x" << height << (pixels == true ? " pixelized" : ""))
    }

    void Delete() {
        if(mCreated) {
            glDeleteTextures(1, &mID);

            mCreated = false;
        }
    }

    ~TextureArrayBuffer() {
        Delete();
    }
};

struct FrameBuffer {
    uint32_t mID, mTextureID;
    bool mCreated = false, mTexCreated = false;

    void Init() {
        if(!mCreated) {
            glGenFramebuffers(1, &mID);

            mCreated = true;
        }
    }

    void Bind() {
        Init();

        glBindFramebuffer(GL_FRAMEBUFFER, mID);
    }

    void Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void GetTextureFromFrameDepthStencil(int width, int height) {
        Bind();

        if(!mTexCreated) {
            glGenTextures(1, &mTextureID);

            mTexCreated = true;
        }

        glBindTexture(GL_TEXTURE_2D, mTextureID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mTextureID, 0);

        Unbind();
    }

    void GetTextureFromFrameDepth(int width, int height) {
        Bind();

        if(!mTexCreated) {
            glGenTextures(1, &mTextureID);

            mTexCreated = true;
        }

        glBindTexture(GL_TEXTURE_2D, mTextureID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mTextureID, 0);

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        Unbind();
    }

    void GetTextureFromFrameColor(int width, int height) {
        Bind();

        if(!mTexCreated) {
            glGenTextures(1, &mTextureID);

            mTexCreated = true;
        }

        glBindTexture(GL_TEXTURE_2D, mTextureID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureID, 0);

        Unbind();
    }

    void Delete() {
        if(mCreated) {
            glDeleteFramebuffers(1, &mID);

            mCreated = false;
        }

        if(mTexCreated) {
            glDeleteTextures(1, &mTextureID);

            mTexCreated = false;
        }
    }

    ~FrameBuffer() {
        Delete();
    }
};

struct RenderBuffer {
    uint32_t mID;
    bool mCreated = false;

    void Init() {
        if(!mCreated) {
            glGenRenderbuffers(1, &mID);

            mCreated = true;
        }
    }

    void Bind() {
        Init();

        glBindRenderbuffer(GL_RENDERBUFFER, mID);
    }

    void Unbind() {
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    void CreateStorage(FrameBuffer fb, int width, int height) {
        Bind();

        fb.Bind();

        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mID);

        fb.GetTextureFromFrameColor(width, height);

        fb.Unbind();
        Unbind();
    }
};

#endif