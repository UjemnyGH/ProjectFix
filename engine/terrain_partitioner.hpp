#pragma once
#ifndef __TERRAIN_PARTITIONER_
#define __TERRAIN_PARTITIONER_

#include <vector>
#include "engine_math.hpp"
#include "render_data_maker.hpp"

class TerrainPartitioner {
private:
    MeshData mMesh;
    std::vector<MeshData> mPartitionedMesh;

public:
    void LoadVertices(MeshData mesh) {
        mMesh = mesh;
    }

    void Partition(uint32_t maxTrianglesAmount) {
        const uint32_t maxTA = maxTrianglesAmount;
        uint32_t counter = 0;

        for(size_t i = 0; i < mMesh.mVertices.size() / 3; i++) {
            
            counter++;
        }
    }
};

#endif