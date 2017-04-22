#include "stdafx.h"

#include "Rasterizer.h"

#include "Primitive.h"

using namespace std;
using namespace glm;
using namespace softlit;

Rasterizer::Rasterizer(const RasterizerSetup& setup)
	: m_setup(setup)
{
	m_frameBuffer.resize(m_setup.viewport.width * m_setup.viewport.height);
	m_depthBuffer.resize(m_setup.viewport.width * m_setup.viewport.height);
}

Rasterizer::~Rasterizer()
{
	m_frameBuffer.clear();
	m_depthBuffer.clear();
}

/*
		- v1
	  -	 -	 -  -
	vo	-	-	- v2
*/
inline float Rasterizer::PixelCoverage(const vec3& a, const vec3& b, const vec2& c)
{
	const int winding = (m_setup.vertexWinding == VertexWinding::COUNTER_CLOCKWISE) ? 1 : -1;
	const float x = (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);

	return winding * x;
}

Rasterizer::Triangle Rasterizer::setupTriangle(const std::vector<glm::vec3>& vbo, const std::vector<uint64_t>& ibo, const uint64_t idx) const
{
	const vec3& v0 = vbo[ibo[idx * 3]];
	const vec3& v1 = vbo[ibo[idx * 3 + 1]];
	const vec3& v2 = vbo[ibo[idx * 3 + 2]];

	return Triangle(v0, v1, v2);
}

void Rasterizer::Draw(Primitive* prim, const mat4& view, const mat4& proj)
{
	// Pre-draw, invalidate frame and depth buffers
	InvalidateBuffers();

	const vector<vec3>& vbo = prim->getVertexBuffer();
	const vector<uint64_t>& ibo = prim->getIndexBuffer();

	// Only raster primitive is triangle
	const uint64_t numTris = prim->getIndexBuffer().size() / 3;
	DBG_ASSERT((prim->getIndexBuffer().size() % 3) == 0);

	for (uint64_t i = 0; i < numTris; i++)
	{
		const Triangle& tri = setupTriangle(vbo, ibo, i);

		// Convert to clip-coordinates
		const vec4 v0Clip = proj * (view * vec4(tri.v0, 1));
		const vec4 v1Clip = proj * (view * vec4(tri.v1, 1));
		const vec4 v2Clip = proj * (view * vec4(tri.v2, 1));

		// Perspective-divide and convert to NDC
		const vec3 v0NDC = v0Clip / v0Clip.w;
		const vec3 v1NDC = v1Clip / v1Clip.w;
		const vec3 v2NDC = v2Clip / v2Clip.w;

		// Now to frame buffer-coordinates
		vec3 v0Raster = { (v0NDC.x + 1) / 2 * m_setup.viewport.width, (1 - v0NDC.y) / 2 * m_setup.viewport.height, v0NDC.z };
		vec3 v1Raster = { (v1NDC.x + 1) / 2 * m_setup.viewport.width, (1 - v1NDC.y) / 2 * m_setup.viewport.height, v1NDC.z };
		vec3 v2Raster = { (v2NDC.x + 1) / 2 * m_setup.viewport.width, (1 - v2NDC.y) / 2 * m_setup.viewport.height, v2NDC.z };

		// 2D clipping
		float xmin = fminf(v0Raster.x, fminf(v1Raster.x, v2Raster.x));
		float xmax = fmaxf(v0Raster.x, fmaxf(v1Raster.x, v2Raster.x));
		float ymin = fminf(v0Raster.y, fminf(v1Raster.y, v2Raster.y));
		float ymax = fmaxf(v0Raster.y, fmaxf(v1Raster.y, v2Raster.y));

		if (xmin + 1 > m_setup.viewport.width || xmax < 0 || ymin + 1 > m_setup.viewport.height || ymax < 0) continue;

		uint32_t xb = max<int>(0, (int32_t)xmin);
		uint32_t xe = min<int>(m_setup.viewport.width - 1, (int32_t)xmax);
		uint32_t yb = max<int>(0, (int32_t)ymin);
		uint32_t ye = min<int>(m_setup.viewport.height - 1, (int32_t)ymax);

		const float triCoverage = PixelCoverage(v0Raster, v1Raster, v2Raster);

		for (uint32_t y = yb; y <= ye; y++)
		{
			for (uint32_t x = xb; x <= xe; x++)
			{
				vec2 sample = { x + 0.5f, y + 0.5f };

				float w0 = PixelCoverage(v1Raster, v2Raster, sample);
				float w1 = PixelCoverage(v2Raster, v0Raster, sample);
				float w2 = PixelCoverage(v0Raster, v1Raster, sample);

				if (w0 >= 0 && w1 >= 0 && w2 >= 0)
				{
					w0 /= triCoverage;
					w1 /= triCoverage;
					w2 = 1.f - w0 - w1;

					float z = ((w0 * v0Raster.z) + (w1 * v1Raster.z) + (w2 * v2Raster.z));
					if (z < m_depthBuffer[y * m_setup.viewport.width + x]) // Depth test; update color & z-buffer if passed
					{
						m_frameBuffer[y * m_setup.viewport.width + x] = vec4(0.5, 0.5, 0.5, 1.0);
						m_depthBuffer[y * m_setup.viewport.width + x] = z;
					}
				}
			}
		}
	}
}