#pragma once
#ifndef __MESH_
#define __MESH_

#include <vector>
#include <algorithm>
#include <iterator>
#include <string>
#include <fstream>
#include <sstream>
#include "core.hpp"
#include "engine_math.hpp"
#include "buffers.hpp"
#include "auto_renderer.hpp"

enum GLTF_Data {
    GLTF_Data_array_buffer = 34962,
    GLTF_Data_element_array_buffer = 34963,
};

struct GLTF_BufferView {
    uint32_t mBuffer;
    uint32_t mByteLength;
    uint32_t mByteOffset;
    uint32_t mTarget;
};

struct MeshData {
    std::vector<float> mVertices;
    std::vector<float> mColors;
    std::vector<float> mTextureCoords;
    std::vector<float> mNormals;
    std::string mPath;

    void Clear() {
        mVertices.clear();
        mColors.clear();
        mTextureCoords.clear();
        mNormals.clear();
    }

    void DebugInfo() {
        for(size_t i = 0; i < mVertices.size() / 3; i++) {
            printf("%lld. V: %f %f %f\n", i, mVertices[i * 3 + 0], mVertices[i * 3 + 1], mVertices[i * 3 + 2]);
        }

        for(size_t i = 0; i < mColors.size() / 4; i++) {
            printf("%lld. C: %f %f %f %f\n", i, mColors[i * 4 + 0], mColors[i * 4 + 1], mColors[i * 4 + 2], mColors[i * 4 + 3]);
        }

        for(size_t i = 0; i < mTextureCoords.size() / 2; i++) {
            printf("%lld. TC: %f %f\n", i, mTextureCoords[i * 2 + 0], mTextureCoords[i * 2 + 1]);
        }

        for(size_t i = 0; i < mNormals.size() / 3; i++) {
            printf("%lld. N: %f %f %f\n", i, mNormals[i * 3 + 0], mNormals[i * 3 + 1], mNormals[i * 3 + 2]);
        }
    }

    void SetColor(RVec color) {
        for(size_t i = 0; i < mColors.size() / 4; i++) {
            mColors[i * 4 + 0] = color.x;
            mColors[i * 4 + 1] = color.y;
            mColors[i * 4 + 2] = color.z;
            mColors[i * 4 + 3] = color.w;
        }
    }

    static struct MeshData LoadOBJModelTriangulated(std::string obj_path) {
        std::ifstream f(obj_path, std::ios::binary);

        if(f.bad()) {
            ENG_WARN("Bad file: " << obj_path);
        }

        if(!f.is_open()) {
            ENG_WARN("File " << obj_path << " is not open");
        }

        std::string line;

        MeshData temp_data;
        MeshData result;

        temp_data.Clear();
        result.Clear();

        while(!f.eof()) {
            std::getline(f, line);

            if(line.find("v ") == 0) {
                float x, y, z;

                std::stringstream ss(line.c_str() + 2);

                ss >> x >> y >> z;

                temp_data.mVertices.push_back(x);
                temp_data.mVertices.push_back(y);
                temp_data.mVertices.push_back(z);
            }
            else if(line.find("vn ") == 0) {
                float x, y, z;

                std::stringstream ss(line.c_str() + 3);

                ss >> x >> y >> z;

                temp_data.mNormals.push_back(x);
                temp_data.mNormals.push_back(y);
                temp_data.mNormals.push_back(z);
            }
            else if(line.find("vt ") == 0) {
                float x, y;

                std::stringstream ss(line.c_str() + 3);

                ss >> x >> y;

                temp_data.mTextureCoords.push_back(x);
                temp_data.mTextureCoords.push_back(y);
            }
            else if(line.find("f ") == 0) {
                uint32_t vx, vy, vz, tx, ty, tz, nx, ny, nz;

                std::string values = line.c_str() + 2;

                for(size_t i = 0; i < values.size(); i++) {
                    if(values[i] == '/') {
                        values[i] = ' ';
                    }
                }

                std::stringstream ss(values.c_str());

                ss >> vx >> tx >> nx >> vy >> ty >> ny >> vz >> tz >> nz;

                result.mVertices.push_back(temp_data.mVertices[(vx - 1) * 3 + 0]);
                result.mVertices.push_back(temp_data.mVertices[(vx - 1) * 3 + 1]);
                result.mVertices.push_back(temp_data.mVertices[(vx - 1) * 3 + 2]);

                result.mVertices.push_back(temp_data.mVertices[(vy - 1) * 3 + 0]);
                result.mVertices.push_back(temp_data.mVertices[(vy - 1) * 3 + 1]);
                result.mVertices.push_back(temp_data.mVertices[(vy - 1) * 3 + 2]);

                result.mVertices.push_back(temp_data.mVertices[(vz - 1) * 3 + 0]);
                result.mVertices.push_back(temp_data.mVertices[(vz - 1) * 3 + 1]);
                result.mVertices.push_back(temp_data.mVertices[(vz - 1) * 3 + 2]);

                result.mNormals.push_back(temp_data.mNormals[(nx - 1) * 3 + 0]);
                result.mNormals.push_back(temp_data.mNormals[(nx - 1) * 3 + 1]);
                result.mNormals.push_back(temp_data.mNormals[(nx - 1) * 3 + 2]);

                result.mNormals.push_back(temp_data.mNormals[(ny - 1) * 3 + 0]);
                result.mNormals.push_back(temp_data.mNormals[(ny - 1) * 3 + 1]);
                result.mNormals.push_back(temp_data.mNormals[(ny - 1) * 3 + 2]);

                result.mNormals.push_back(temp_data.mNormals[(nz - 1) * 3 + 0]);
                result.mNormals.push_back(temp_data.mNormals[(nz - 1) * 3 + 1]);
                result.mNormals.push_back(temp_data.mNormals[(nz - 1) * 3 + 2]);

                result.mTextureCoords.push_back(temp_data.mTextureCoords[(tx - 1) * 2 + 0]);
                result.mTextureCoords.push_back(temp_data.mTextureCoords[(tx - 1) * 2 + 1]);

                result.mTextureCoords.push_back(temp_data.mTextureCoords[(ty - 1) * 2 + 0]);
                result.mTextureCoords.push_back(temp_data.mTextureCoords[(ty - 1) * 2 + 1]);

                result.mTextureCoords.push_back(temp_data.mTextureCoords[(tz - 1) * 2 + 0]);
                result.mTextureCoords.push_back(temp_data.mTextureCoords[(tz - 1) * 2 + 1]);
            }
        }

        f.close();

        for(size_t i = 0; i < result.mVertices.size() / 3; i++) {
            result.mColors.push_back(1.0f);
            result.mColors.push_back(1.0f);
            result.mColors.push_back(1.0f);
            result.mColors.push_back(1.0f);
        }

        result.mPath = obj_path;

        return result;
    }

    // Dont work yet
    static struct MeshData SimpleLoadGLTFModel(std::string gltf_path) {
        std::string gltf_json = LoadFile(gltf_path);

        uint8_t pos_buf_view = gltf_json[gltf_json.find("\"POSITION\":") + 11] - '0';
        uint8_t norm_buf_view = gltf_json[gltf_json.find("\"NORMAL\":") + 9] - '0';
        uint8_t tex_buf_view = gltf_json[gltf_json.find("\"TEXCOORD_0\":") + 13] - '0';
        uint8_t indices_buf_view = gltf_json[gltf_json.find("\"indices\":") + 10] - '0';

        uint32_t current_buffer_view = 0;
        uint32_t current_pos = 0;

        current_pos = gltf_json.find("\"bufferViews\":") + 14;

        if(current_pos < 14) {
            ENG_ERR("bufferViews not found\n")

            return MeshData();
        }

        std::string buffer;

        std::vector<GLTF_BufferView> buffer_views;

        while(gltf_json[current_pos] != ']') {
            if(gltf_json[current_pos] == '}') {
                buffer_views.push_back(GLTF_BufferView());

                uint32_t start_bid = buffer.find("\"buffer\":") + 9;
                uint32_t end_bid = buffer.find(",", start_bid);

                uint32_t start_blen = buffer.find("\"byteLength\":") + 13;
                uint32_t end_blen = buffer.find(",", start_blen);

                uint32_t start_boff = buffer.find("\"byteOffset\":") + 13;
                uint32_t end_boff = buffer.find(",", start_boff);

                uint32_t start_btar = buffer.find("\"target\":") + 9;
                uint32_t end_btar = buffer.find("\n", start_btar);

                char *buff = &buffer[start_bid];
                char *buffLen = &buffer[start_blen];
                char *buffOff = &buffer[start_boff];
                char *buffTar = &buffer[start_btar];

                buffer_views[buffer_views.size() - 1].mBuffer = CharsToInt(buff, end_bid - start_bid);
                buffer_views[buffer_views.size() - 1].mByteLength = CharsToInt(buffLen, end_blen - start_blen);
                buffer_views[buffer_views.size() - 1].mByteOffset = CharsToInt(buffOff, end_boff - start_boff);
                buffer_views[buffer_views.size() - 1].mTarget = CharsToInt(buffTar, end_btar - start_btar);

                buffer.clear();
                current_buffer_view++;
            }

            buffer += gltf_json[current_pos++];
        }

        current_pos = gltf_json.find("\"uri\":\"") + 7;

        buffer.clear();

        std::copy(gltf_json.begin() + current_pos, gltf_json.begin() + gltf_json.find_first_of('\"', current_pos), std::back_inserter(buffer));

        std::vector<float> values;
        std::vector<uint16_t> indices;

        if(buffer.find("base64") != (size_t)-1) {
            std::string base64;

            std::copy(buffer.begin() + buffer.find("base64") + 7, buffer.end(), std::back_inserter(base64));

            values = ConvertBytesTo<float>(DecodeBase64AsBytes(base64));
            indices = ConvertBytesTo<uint16_t>(DecodeBase64AsBytes(base64), buffer_views[indices_buf_view].mByteOffset);
        }
        else {
            values = ConvertBytesFromFileTo<float>(buffer);
            indices = ConvertBytesFromFileTo<uint16_t>(buffer, buffer_views[indices_buf_view].mByteOffset);
        }

        for(size_t i = 0; i < indices.size(); i++) {
            ENG_DEBUG(indices[i]);
        }

        for(size_t i = 0; i < values.size(); i++) {
            ENG_DEBUG(values[i]);
        }

        MeshData temp_data;

        int byte_offset = buffer_views[pos_buf_view].mByteOffset / 24;
        int byte_length = buffer_views[pos_buf_view].mByteLength / 24;

        for(int i = byte_offset; i < byte_offset + byte_length; i++) {
            ENG_DEBUG("Current vertices index: " << indices[(i - byte_offset) * 3] + 0 + byte_offset * 9 - 1 << " I = " << (i - byte_offset) * 3)
            temp_data.mVertices.push_back(values[indices[(i - byte_offset) * 3] + 0 + byte_offset * 9]);
            temp_data.mVertices.push_back(values[indices[(i - byte_offset) * 3] + 1 + byte_offset * 9]);
            temp_data.mVertices.push_back(values[indices[(i - byte_offset) * 3] + 2 + byte_offset * 9]);
        }

        byte_offset = buffer_views[norm_buf_view].mByteOffset / 24;
        byte_length = buffer_views[norm_buf_view].mByteLength / 24;

        for(int i = byte_offset; i < byte_offset + byte_length; i++) {
            ENG_DEBUG("Current normals index: " << indices[(i - byte_offset) * 3 + 1] + 0 + byte_offset * 9 - 1 << " I = " << (i - byte_offset) * 3 + 1)
            temp_data.mNormals.push_back(values[indices[(i - byte_offset) * 3 + 1] + 0 + byte_offset * 9]);
            temp_data.mNormals.push_back(values[indices[(i - byte_offset) * 3 + 1] + 1 + byte_offset * 9]);
            temp_data.mNormals.push_back(values[indices[(i - byte_offset) * 3 + 1] + 2 + byte_offset * 9]);
        }

        byte_offset = buffer_views[tex_buf_view].mByteOffset / 16;
        byte_length = buffer_views[tex_buf_view].mByteLength / 16;

        for(int i = byte_offset; i < byte_offset + byte_length; i++) {
            ENG_DEBUG("Current texture coordinates index: " << indices[(i - byte_offset) * 2 + 2] + 0 + byte_offset * 4 - 1 << " I = " << (i - byte_offset) * 2 + 2)
            temp_data.mTextureCoords.push_back(values[indices[(i - byte_offset) * 2 + 2] + 0 + byte_offset * 4]);
            temp_data.mTextureCoords.push_back(values[indices[(i - byte_offset) * 2 + 2] + 1 + byte_offset * 4]);
        }

        for(size_t i = 0; i < (temp_data.mVertices.size() / 3) * 4; i++) {
            temp_data.mColors.push_back(1.0f);
        }

        ENG_INFO("Loaded model from \"" << gltf_path << "\"")
        ENG_WARN("Returning temporary data!");
        return temp_data;
    }
};

class Mesh : public EObject {
public:
    ShaderProgram mShaderProgram;
    VertexArray mArray;
    VertexBuffer mVerticesBuffer;
    VertexBuffer mColorBuffer;
    VertexBuffer mTextureBuffer;
    VertexBuffer mNormalsBuffer;
    VertexBuffer mTextureIDBuffer;
    Transform mMeshTransform;
    Shader mVertexShader;
    Shader mFragmentShader;
    
    MeshData mMeshData;
    std::vector<float> mTextureID = { 33 };

    std::string mTexturePath;

    void InitializeRenderMesh() {
        mVertexShader.FileShader(SHADER_ASSET(light.vert), GL_VERTEX_SHADER);
        mFragmentShader.FileShader(SHADER_ASSET(light.frag), GL_FRAGMENT_SHADER);

        mShaderProgram.Attach(mVertexShader.mID);
        mShaderProgram.Attach(mFragmentShader.mID);
        mShaderProgram.Link();

        mShaderProgram.Use();

        mShaderProgram.SetInt1Ptr("u_TexturesArray", 32, gTextureSamplers);

        mShaderProgram.Unuse();

        mArray.Bind();

        mVerticesBuffer.BindDataToPlace(0, 3);
        mColorBuffer.BindDataToPlace(1, 4);
        mTextureBuffer.BindDataToPlace(2, 2);
        mNormalsBuffer.BindDataToPlace(3, 3);
        mTextureIDBuffer.BindDataToPlace(4, 1);

        mArray.Unbind();
    }

    void RenderMesh() {
        mShaderProgram.Use();
        mArray.Bind();

        mShaderProgram.SetFloatMat4("u_Transform", mMeshTransform.GetTransform().m);

        glDrawArrays(GL_TRIANGLES, 0, mMeshData.mVertices.size() / 3);

        mArray.Unbind();
        mShaderProgram.Unuse();
    }
};

#endif