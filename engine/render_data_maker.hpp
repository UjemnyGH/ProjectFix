#pragma once
#ifndef __RENDER_DATA_MAKER_
#define __RENDER_DATA_MAKER_

#include "core.hpp"
#include "mesh.hpp"

struct RenderData {
    std::vector<MeshData> mOriginalData;
    std::vector<MeshData> mTransformedData;
    MeshData mJoinedData;
    std::vector<Transform> mTransform;
    std::vector<uint64_t> mDataSizes;
    std::vector<uint64_t> mDataEnds;
    std::vector<float> mTextureIDs;
    std::map<std::string, uint32_t> mMeshIDByPath;

    void SetTextureCoordinatesWrapping(uint32_t id, RVec texCoords) {
        for(size_t i = 0; i < mOriginalData[id].mTextureCoords.size() / 2; i++) {
            mJoinedData.mTextureCoords[(mDataEnds[id] * 2 - mDataSizes[id] * 2) + (i * 2 + 0)] = mTransformedData[id].mTextureCoords[i * 2 + 0] = mOriginalData[id].mTextureCoords[i * 2 + 0] / texCoords.x;
            mJoinedData.mTextureCoords[(mDataEnds[id] * 2 - mDataSizes[id] * 2) + (i * 2 + 1)] = mTransformedData[id].mTextureCoords[i * 2 + 1] = mOriginalData[id].mTextureCoords[i * 2 + 1] / texCoords.y;
        }  
    }

    void SetTextureID(uint32_t objectId, uint32_t textureId) {
        for(uint64_t i = mDataEnds[objectId] - mDataSizes[objectId]; i < mDataEnds[objectId]; i++) {
            mTextureIDs[i] = (float)textureId;
        }

        ENG_DEBUG("Object by ID = " << objectId << " now have texture ID = " << textureId);
    }

    void RejoinData() {
        mJoinedData.Clear();

        for(MeshData md : mTransformedData) {
            std::copy(md.mVertices.begin(), md.mVertices.end(), std::back_inserter(mJoinedData.mVertices));
            std::copy(md.mColors.begin(), md.mColors.end(), std::back_inserter(mJoinedData.mColors));
            std::copy(md.mTextureCoords.begin(), md.mTextureCoords.end(), std::back_inserter(mJoinedData.mTextureCoords));
            std::copy(md.mNormals.begin(), md.mNormals.end(), std::back_inserter(mJoinedData.mNormals));
        }

        ENG_INFO("Rejoined " << mJoinedData.mVertices.size() / 9 << " triangles");
    }

    void AddData(MeshData data) {
        mMeshIDByPath.insert(std::make_pair(data.mPath, mOriginalData.size()));
        mOriginalData.push_back(data);
        mTransformedData.push_back(data);
        mTransform.push_back(Transform());

        mDataSizes.push_back(data.mVertices.size() / 3);
        mDataEnds.push_back((mDataEnds.size() > 0 ? mDataEnds[mDataEnds.size() - 1] : 0) + (data.mVertices.size() / 3));
    
        for(uint64_t i = 0; i < mDataSizes[mDataSizes.size() - 1]; i++) {
            mTextureIDs.push_back(33.0f);
        }

        std::copy(data.mVertices.begin(), data.mVertices.end(), std::back_inserter(mJoinedData.mVertices));
        std::copy(data.mColors.begin(), data.mColors.end(), std::back_inserter(mJoinedData.mColors));
        std::copy(data.mTextureCoords.begin(), data.mTextureCoords.end(), std::back_inserter(mJoinedData.mTextureCoords));
        std::copy(data.mNormals.begin(), data.mNormals.end(), std::back_inserter(mJoinedData.mNormals));

        ENG_INFO("Added data with " << data.mVertices.size() / 9 << " triangles");

        TransformData(mOriginalData.size() - 1);
    }

    void RemoveData(uint32_t id) {
        ENG_INFO("Removed data of size" << mOriginalData[id].mVertices.size() / 3);
        mMeshIDByPath.erase(mOriginalData[id].mPath);
        mOriginalData.erase(mOriginalData.begin() + id);
        mTransformedData.erase(mTransformedData.begin() + id);
        mTransform.erase(mTransform.begin() + id);
        
        for(uint64_t i = mDataEnds[id] - mDataSizes[id]; i < mDataEnds[id]; i++) {
            mTextureIDs.erase(mTextureIDs.begin() + i);
        }

        mDataSizes.erase(mDataSizes.begin() + id);

        mDataEnds.clear();

        for(auto size : mDataSizes) {
            mDataEnds.push_back((mDataEnds.size() > 0 ? mDataEnds[mDataEnds.size() - 1] : 0) + size);
        }

        RejoinData();
    }

    void TransformData(uint32_t id) {
        //ENG_INFO("Data ends at " << mDataEnds[id] << " sized to " << mDataSizes[id])
        
        for(size_t i = 0; i < mOriginalData[id].mVertices.size() / 3; i++) {
            RVec original = RVec(mOriginalData[id].mVertices[i * 3 + 0], mOriginalData[id].mVertices[i * 3 + 1], mOriginalData[id].mVertices[i * 3 + 2], 1.0f);
            RVec transformed = mTransform[id].GetTransform() * original;

            mJoinedData.mVertices[(mDataEnds[id] * 3 - mDataSizes[id] * 3) + (i * 3 + 0)] = mTransformedData[id].mVertices[i * 3 + 0] = transformed.x;
            mJoinedData.mVertices[(mDataEnds[id] * 3 - mDataSizes[id] * 3) + (i * 3 + 1)] = mTransformedData[id].mVertices[i * 3 + 1] = transformed.y;
            mJoinedData.mVertices[(mDataEnds[id] * 3 - mDataSizes[id] * 3) + (i * 3 + 2)] = mTransformedData[id].mVertices[i * 3 + 2] = transformed.z;
        }
    }

    void TransformAllData() {
        for(size_t i = 0; i < mOriginalData.size(); i++) {
            TransformData(i);
        }
    }
};

#endif