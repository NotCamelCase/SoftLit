#pragma once

#include "MathUtils.h"

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

	struct RasterizerSetup
	{

		CullMode cullMode = CullMode::CULL_DISABLED;
		VertexWinding vertexWinding = VertexWinding::CLOCKWISE;

		Viewport viewport = { 0.f, 0.f, 1024u, 768u };
	};

	class Rasterizer
	{
	public:
		Rasterizer(const RasterizerSetup& setup);
		~Rasterizer();

		const std::vector<vec4>& getColorBuffer() const
		{
			return m_colorBuffer;
		}

		const std::vector<float>& getZBuffer() const
		{
			return m_zBuffer;
		}

		/*
		* Rasterize given primitive set
		*/
		void Draw(const std::vector<vec3>& vertices, const mat4& view, const mat4& proj);

	private:
		std::vector<vec4> m_colorBuffer; // Used to hold rasterized primitives color buffer, colors in [0.f, 1.f]
		std::vector<float> m_zBuffer; // Used as depth buffer

		RasterizerSetup m_setup;

		float PixelCoverage(const vec3& a, const vec3& b, const vec2& c);
	};
}