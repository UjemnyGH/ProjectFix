#pragma once
#ifndef __AUTO_RENDERER_
#define __AUTO_RENDERER_

#include "core.hpp"
#include "buffers.hpp"
#include "bitmap_images.hpp"

AutoMap<TextureArrayBuffer> gTexturizer;

void GlobalAutoRenderer(ShaderProgram* pSp, VertexArray* pVa, uint32_t triangles) {
    pSp->Use();
    pVa->Bind();

    glDrawArrays(GL_TRIANGLES, 0, triangles);

    pSp->Unuse();
    pVa->Unbind();
}

#endif