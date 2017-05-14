#pragma once

#include "stdafx.h"

using namespace std;
using namespace glm;
using namespace softlit;

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
vec4 FS_Simple(const UBO* const ubo, const Vertex_OUT* const in)
{
	const vec3 outcol = in->attrib_vec3[0] * 0.5f + 0.5f;

	return vec4(outcol, 1);
}