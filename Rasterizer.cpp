#include "stdafx.h"

#include "Rasterizer.h"

using namespace std;
using namespace softlit;

Rasterizer::Rasterizer(const RasterizerSetup& setup)
	: m_setup(setup)
{
	m_colorBuffer.resize(m_setup.viewport.width * m_setup.viewport.height);
	m_zBuffer.resize(m_setup.viewport.width * m_setup.viewport.height);

	const uint32_t viewportDim = m_setup.viewport.width * m_setup.viewport.height;

	//TODO: Assign a clear_color
	for (uint32_t i = 0; i < viewportDim; i++)
	{
		m_colorBuffer[i] = 0.f;
	}

	// Init z-buffer to FLT_MAX for intiial comparisons
	for (uint32_t i = 0; i < viewportDim; i++)
	{
		m_zBuffer[i] = FLT_MAX;
	}
}

Rasterizer::~Rasterizer()
{
	m_colorBuffer.clear();
	m_zBuffer.clear();
}

/*
		- v1
	  -	 -	 -  -
	vo	-	-	- v2

*/
float Rasterizer::PixelCoverage(const vec3& a, const vec3& b, const vec2& c)
{
	return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

void Rasterizer::Draw(const vector<vec3>& vertices, const mat4& view, const mat4& proj)
{
	const size_t numTris = vertices.size() / 3;
	for (size_t i = 0; i < numTris; i += 3)
	{
		const vec3 v0 = vertices[i];
		const vec3 v1 = vertices[i + 1];
		const vec3 v2 = vertices[i + 2];

		// Convert to clip-coordinates
		vec4 v0Clip = proj * (view * vec4(v0, 1));
		vec4 v1Clip = proj * (view * vec4(v1, 1));
		vec4 v2Clip = proj * (view * vec4(v2, 1));

		// Convert to NDC-space normalizing by w-divide
		vec3 v0NDC = v0Clip / v0Clip.w;
		vec3 v1NDC = v1Clip / v1Clip.w;
		vec3 v2NDC = v2Clip / v2Clip.w;

		// Now to frame buffer-coordin
		vec3 v0Raster = { m_setup.viewport.width * (v0NDC.x + 1) / 2, m_setup.viewport.height * (1 - v0NDC.y) / 2, v0NDC.z };
		vec3 v1Raster = { m_setup.viewport.width * (v1NDC.x + 1) / 2, m_setup.viewport.height * (1 - v1NDC.y) / 2, v1NDC.z };
		vec3 v2Raster = { m_setup.viewport.width * (v2NDC.x + 1) / 2, m_setup.viewport.height * (1 - v2NDC.y) / 2, v2NDC.z };

		// Per-triangle variables
		const float triArea = PixelCoverage(v0Raster, v1Raster, v2Raster);
		const float v0OverZ = 1.f / v0Raster.z;
		const float v1OverZ = 1.f / v1Raster.z;
		const float v2OverZ = 1.f / v2Raster.z;

		for (uint32_t j = 0; j < m_setup.viewport.height; j++)
		{
			for (uint32_t i = 0; i < m_setup.viewport.width; i++)
			{
				vec2 sample = { i + 0.5f, j + 0.5f };
				float delta0 = PixelCoverage(v1Raster, v2Raster, sample);
				float delta1 = PixelCoverage(v2Raster, v0Raster, sample);
				float delta2 = PixelCoverage(v0Raster, v2Raster, sample);

				if (delta0 >= 0.f && delta1 >= 0.f && delta2 >= 0.f)
				{
					delta0 /= triArea;
					delta1 /= triArea;
					delta2 /= triArea;

					float z = 1.f / ((delta0 * v0OverZ) + (delta1 * v1OverZ) + (delta2 * v2OverZ));
					if (z < m_zBuffer[j * m_setup.viewport.width + i]) // Depth test, update color & z-buffer if passed
					{
						m_colorBuffer[j * m_setup.viewport.width + i] = vec4(1.f);
						m_zBuffer[j * m_setup.viewport.width + i] = z;
					}
				}
			}
		}
	}
}