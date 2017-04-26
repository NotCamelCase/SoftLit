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
inline float Rasterizer::PixelCoverage(const vec2& a, const vec2& b, const vec2& c)
{
	const int winding = (m_setup.vertexWinding == VertexWinding::COUNTER_CLOCKWISE) ? 1 : -1;
	const float x = (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);

	return winding * x;
}

void Rasterizer::setupTriangle(Primitive* prim, const uint64_t idx, glm::vec3& v0, glm::vec3& v1, glm::vec3& v2) const
{
	const VertexBuffer& vbo = prim->getVertexBuffer();
	const IndexBuffer ibo = prim->getIndexBuffer();

	if (prim->getPrimitiveTopology() == PrimitiveTopology::TRIANGLE_LIST)
	{
		v0 = vbo[ibo[idx * 3]];
		v1 = vbo[ibo[idx * 3 + 1]];
		v2 = vbo[ibo[idx * 3 + 2]];
	}
	else
		DBG_ASSERT(0 && "Primitive topology not implemented");
}

bool softlit::Rasterizer::clip2D(const glm::vec3 & v0, const glm::vec3 & v1, const glm::vec3 & v2, Viewport& vp) const
{
	float xmin = fminf(v0.x, fminf(v1.x, v2.x));
	float xmax = fmaxf(v0.x, fmaxf(v1.x, v2.x));
	float ymin = fminf(v0.y, fminf(v1.y, v2.y));
	float ymax = fmaxf(v0.y, fmaxf(v1.y, v2.y));

	// Early-out when viewport bounds exceeded
	if (xmin + 1 > m_setup.viewport.width || xmax < 0 || ymin + 1 > m_setup.viewport.height || ymax < 0) return false;

	vp.x = max<int>(0, (int32_t)xmin);
	vp.width = min<int>(m_setup.viewport.width - 1, (int32_t)xmax);
	vp.y = max<int>(0, (int32_t)ymin);
	vp.height = min<int>(m_setup.viewport.height - 1, (int32_t)ymax);

	return true;
}

void Rasterizer::Draw(Primitive* prim, const mat4& view, const mat4& proj)
{
	// Pre-draw, invalidate frame and depth buffers
	InvalidateBuffers();

	// Only raster primitive is triangle
	const uint64_t numTris = prim->getIndexBuffer().size() / 3;
	DBG_ASSERT((prim->getIndexBuffer().size() % 3) == 0);

	for (uint64_t i = 0; i < numTris; i++)
	{
		vec3 v0, v1, v2;
		setupTriangle(prim, i, v0, v1, v2);

		const vertex_shader VS = prim->VS();
		DBG_ASSERT(VS && "invalid vertex_shader!");

		UniformBuffer ubo = prim->UBO();
		DBG_ASSERT(ubo && "Primitive UBO not supplied!");

		// Execute VS for each vertex
		const vec4 v0Clip = VS(v0, ubo);
		const vec4 v1Clip = VS(v1, ubo);
		const vec4 v2Clip = VS(v2, ubo);

		// Perspective-divide and convert to NDC
		const vec3 v0NDC = v0Clip / v0Clip.w;
		const vec3 v1NDC = v1Clip / v1Clip.w;
		const vec3 v2NDC = v2Clip / v2Clip.w;

		// Now to frame buffer-coordinates
		vec3 v0Raster = { (v0NDC.x + 1) / 2 * m_setup.viewport.width, (1 - v0NDC.y) / 2 * m_setup.viewport.height, v0NDC.z };
		vec3 v1Raster = { (v1NDC.x + 1) / 2 * m_setup.viewport.width, (1 - v1NDC.y) / 2 * m_setup.viewport.height, v1NDC.z };
		vec3 v2Raster = { (v2NDC.x + 1) / 2 * m_setup.viewport.width, (1 - v2NDC.y) / 2 * m_setup.viewport.height, v2NDC.z };

		Viewport vp;
		if (!clip2D(v0Raster, v1Raster, v2Raster, vp)) continue;

		const float triCoverage = PixelCoverage(v0Raster, v1Raster, v2Raster);

		for (uint32_t y = vp.y; y <= vp.height; y++)
		{
			for (uint32_t x = vp.x; x <= vp.width; x++)
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
						const vec4 final_fragment = prim->FS()();

						m_frameBuffer[y * m_setup.viewport.width + x] = final_fragment;
						m_depthBuffer[y * m_setup.viewport.width + x] = z;
					}
				}
			}
		}
	}
}