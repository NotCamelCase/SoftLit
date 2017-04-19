#pragma once

#include "stdafx.h"

namespace softlit
{
	enum class CullMode
	{
		CULL_DISABLED, // Culling disabled
		CULL_BACK, // Cull back-facing prims
		CULL_FACE // Cull front-facing prims
	};

	enum class VertexWinding
	{
		CLOCKWISE,
		COUNTER_CLOCKWISE // Default
	};

	struct Viewport
	{
		float x;
		float y;

		uint32_t width;
		uint32_t height;
	};

	struct RasterizerSetup
	{
		CullMode cullMode = CullMode::CULL_DISABLED;
		VertexWinding vertexWinding = VertexWinding::CLOCKWISE;

		Viewport viewport = { 0.f, 0.f, 1024u, 768u };
	};
}