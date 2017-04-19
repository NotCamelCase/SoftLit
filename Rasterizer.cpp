#include "stdafx.h"

#include "Rasterizer.h"

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
float Rasterizer::PixelCoverage(const vec3& a, const vec3& b, const vec2& c)
{
	const int winding = (m_setup.vertexWinding == VertexWinding::COUNTER_CLOCKWISE) ? 1 : -1;
	const float x = (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);

	return winding * x;
}

void Rasterizer::Draw(const vector<vec3>& vertices, const mat4& view, const mat4& proj)
{
	// Pre-draw, invalidate frame and depth buffers
	InvalidateBuffers();

	const size_t numTris = vertices.size() / 3;
	for (size_t i = 0; i < numTris; i++)
	{
		const vec3& v0 = vertices[i * 3];
		const vec3& v1 = vertices[i * 3 + 1];
		const vec3& v2 = vertices[i * 3 + 2];

		// Convert to clip-coordinates
		const vec4 v0Clip = proj * (view * vec4(v0, 1));
		const vec4 v1Clip = proj * (view * vec4(v1, 1));
		const vec4 v2Clip = proj * (view * vec4(v2, 1));

		// Perspective-divide and convert to NDC
		const vec3 v0NDC = v0Clip / v0Clip.w;
		const vec3 v1NDC = v1Clip / v1Clip.w;
		const vec3 v2NDC = v2Clip / v2Clip.w;

		// Now to frame buffer-coordinates
		vec3 v0Raster = { (v0NDC.x + 1) / 2 * m_setup.viewport.width, (1 - v0NDC.y) / 2 * m_setup.viewport.height, v0NDC.z };
		vec3 v1Raster = { (v1NDC.x + 1) / 2 * m_setup.viewport.width, (1 - v1NDC.y) / 2 * m_setup.viewport.height, v1NDC.z };
		vec3 v2Raster = { (v2NDC.x + 1) / 2 * m_setup.viewport.width, (1 - v2NDC.y) / 2 * m_setup.viewport.height, v2NDC.z };

		// Per-triangle variables
		const float triCoverage = PixelCoverage(v0Raster, v1Raster, v2Raster);

		for (uint32_t j = 0; j < m_setup.viewport.height; j++)
		{
			for (uint32_t i = 0; i < m_setup.viewport.width; i++)
			{
				vec2 sample = { i + 0.5f, j + 0.5f };

				float w0 = PixelCoverage(v1Raster, v2Raster, sample);
				float w1 = PixelCoverage(v2Raster, v0Raster, sample);
				float w2 = PixelCoverage(v0Raster, v1Raster, sample);

				if (w0 >= 0 && w1 >= 0 && w2 >= 0)
				{
					w0 /= triCoverage;
					w1 /= triCoverage;
					w2 /= triCoverage;

					float z = 1.f / ((w0 * v0Raster.z) + (w1 * v1Raster.z) + (w2 * v2Raster.z));
					if (z < m_depthBuffer[j * m_setup.viewport.width + i]) // Depth test, update color & z-buffer if passed
					{
						m_frameBuffer[j * m_setup.viewport.width + i] = vec4(0.5, 0.5, 0.5, 1.0);
						m_depthBuffer[j * m_setup.viewport.width + i] = z;
					}
				}
			}
		}
	}
}