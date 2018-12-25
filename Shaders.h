#pragma once

#include "stdafx.h"

#include "Texture.h"

using namespace glm;

namespace softlit
{
    struct UBO
    {
        mat4 MV;
        mat4 MVP;
    };

    // VS: Out normal
    vec4 VS_Simple(const glm::vec3& pos, const UBO* const ubo, const Vertex_IN* const in, Vertex_OUT* out)
    {
        out->PushVertexAttribute(in->attrib_vec3[0]); // Push normal

        return (ubo->MVP * vec4(pos, 1));
    }

    // FS: Render scaled vertex normals
    vec4 FS_Simple(const UBO* const ubo, const Vertex_OUT* const in, Texture** tbo)
    {
        const vec3 outcol = in->attrib_vec3[0] * 0.5f + 0.5f;

        return vec4(outcol, 1);
    }

    vec4 VS_Textured(const glm::vec3& pos, const UBO* const ubo, const Vertex_IN* const in, Vertex_OUT* out)
    {
        out->PushVertexAttribute(in->attrib_vec2[0]); // Push uv

        return (ubo->MVP * vec4(pos, 1));
    }

    vec4 FS_Textured(const UBO* const ubo, const Vertex_OUT* const in, Texture** tbo)
    {
        const vec3 texcol = tbo[0]->Sample<TextureSampler::SAMPLE_RGB>(in->attrib_vec2[0]);

        return vec4(texcol, 1);
    }
}