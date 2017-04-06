#pragma once

#include "MathUtils.h"

namespace softlit
{
	struct RasterizerSetup
	{
		enum class CullMode
		{
			CULL_DISABLED, // Culling disabled
			CULL_BACK, // Cull back-facing prims
			CULL_FACE // Cull front-facing prims
		};

		enum class VertexWinding
		{
			CLOCKWISE, // Default
			COUNTER_CLOCKWISE
		};

		struct Viewport
		{
			float x;
			float y;

			uint32_t width;
			uint32_t height;
		};

		CullMode cullMode = CullMode::CULL_DISABLED;
		VertexWinding vertexWinding = VertexWinding::CLOCKWISE;

		Viewport viewport = { 0.f, 0.f, 1024u, 768u };
	};

	class Rasterizer
	{
	public:
		Rasterizer(const RasterizerSetup& setup);
		~Rasterizer();

		const std::vector<uchar>& getColorBuffer() const
		{
			return m_colorBuffer;
		}

		const std::vector<uchar>& getZBuffer() const
		{
			return m_zBuffer;
		}

		/*
		* Rasterize given primitive as a set of vertices
		*/
		void Draw(const std::vector<vec3>& vertices, const mat4& view, const mat4& proj);

	private:
		std::vector<uchar> m_colorBuffer; // Used to hold rasterized frame
		std::vector<uchar> m_zBuffer; // Used as depth buffer

		RasterizerSetup m_setup;
	};
}