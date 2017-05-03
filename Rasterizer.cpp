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
inline float Rasterizer::PixelCoverage(const vec2& a, const vec2& b, const vec2& c)
{
	const int winding = (m_setup.vertexWinding == VertexWinding::COUNTER_CLOCKWISE) ? 1 : -1;
	const float x = (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);

	return winding * x;
}

void Rasterizer::SetupTriangle(Primitive* prim, const uint64_t idx, glm::vec3& v0, glm::vec3& v1, glm::vec3& v2) const
{
	const VertexBuffer& vbo = prim->getVertexBuffer();
	const IndexBuffer& ibo = prim->getIndexBuffer();

	v0 = vbo[ibo[idx * 3]];
	v1 = vbo[ibo[idx * 3 + 1]];
	v2 = vbo[ibo[idx * 3 + 2]];
}

bool softlit::Rasterizer::clip2D(const glm::vec2 & v0, const glm::vec2 & v1, const glm::vec2 & v2, Viewport& vp) const
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

void Rasterizer::InterpolateAttributes(const float z, const float w0, const float w1, const float w2, const Vertex_OUT& out0, const Vertex_OUT& out1, const Vertex_OUT& out2, Vertex_OUT& attribs)
{
	if (!out0.attrib_vec4.empty())
	{
		DBG_ASSERT(!out1.attrib_vec4.empty());
		DBG_ASSERT(!out2.attrib_vec4.empty());

		DBG_ASSERT(out0.attrib_vec4.size() == out2.attrib_vec4.size());
		DBG_ASSERT(out1.attrib_vec4.size() == out2.attrib_vec4.size());

		for (int i = 0; i < out0.attrib_vec4.size(); i++)
		{
			const vec4& attr0 = out0.attrib_vec4[i];
			const vec4& attr1 = out1.attrib_vec4[i];
			const vec4& attr2 = out2.attrib_vec4[i];

			vec3 attrib = w0 * attr0 + w1 * attr1 + w2 * attr2;

			attribs.PushVertexAttribute(attrib * z);
		}
	}

	if (!out0.attrib_vec3.empty())
	{
		DBG_ASSERT(!out1.attrib_vec3.empty());
		DBG_ASSERT(!out2.attrib_vec3.empty());

		DBG_ASSERT(out0.attrib_vec3.size() == out2.attrib_vec3.size());
		DBG_ASSERT(out1.attrib_vec3.size() == out2.attrib_vec3.size());

		for (int i = 0; i < out0.attrib_vec3.size(); i++)
		{
			const vec3& attr0 = out0.attrib_vec3[i];
			const vec3& attr1 = out1.attrib_vec3[i];
			const vec3& attr2 = out2.attrib_vec3[i];

			vec3 attrib = w0 * attr0 + w1 * attr1 + w2 * attr2;

			attribs.PushVertexAttribute(attrib * z);
		}
	}

	if (!out0.attrib_vec2.empty())
	{
		DBG_ASSERT(!out1.attrib_vec2.empty());
		DBG_ASSERT(!out2.attrib_vec2.empty());

		DBG_ASSERT(out0.attrib_vec2.size() == out2.attrib_vec2.size());
		DBG_ASSERT(out1.attrib_vec2.size() == out2.attrib_vec2.size());

		for (int i = 0; i < out0.attrib_vec2.size(); i++)
		{
			const vec2& attr0 = out0.attrib_vec2[i];
			const vec2& attr1 = out1.attrib_vec2[i];
			const vec2& attr2 = out2.attrib_vec2[i];

			vec2 attrib = w0 * attr0 + w1 * attr1 + w2 * attr2;

			attribs.PushVertexAttribute(attrib * z);
		}
	}
}

void Rasterizer::FetchVertexAttributes(Primitive* prim, uint64_t idx, Vertex_IN& in0, Vertex_IN& in1, Vertex_IN& in2)
{
	const VertexAttributes& attribs = prim->getVertexAttributes();

	// Fetch attributes of type vec4
	if (!attribs.attrib_vec4.empty()) // At least one vec4 attribute buffer created
	{
		for (int i = 0; i < attribs.attrib_vec4.size(); i++)
		{
			in0.PushVertexAttribute(attribs.attrib_vec4[i][idx * 3]);
			in1.PushVertexAttribute(attribs.attrib_vec4[i][idx * 3 + 1]);
			in2.PushVertexAttribute(attribs.attrib_vec4[i][idx * 3 + 2]);
		}
	}

	// Fetch attributes of type vec3
	if (!attribs.attrib_vec3.empty()) // At least one vec4 attribute buffer created
	{
		for (int i = 0; i < attribs.attrib_vec3.size(); i++)
		{
			in0.PushVertexAttribute(attribs.attrib_vec3[i][idx * 3]);
			in1.PushVertexAttribute(attribs.attrib_vec3[i][idx * 3 + 1]);
			in2.PushVertexAttribute(attribs.attrib_vec3[i][idx * 3+ 2]);
		}
	}

	// Fetch attributes of type vec2
	if (!attribs.attrib_vec2.empty()) // At least one vec4 attribute buffer created
	{
		for (int i = 0; i < attribs.attrib_vec2.size(); i++)
		{
			in0.PushVertexAttribute(attribs.attrib_vec2[i][idx * 3]);
			in1.PushVertexAttribute(attribs.attrib_vec2[i][idx * 3 + 1]);
			in2.PushVertexAttribute(attribs.attrib_vec2[i][idx * 3 + 2]);
		}
	}
}

void Rasterizer::Draw(Primitive* prim)
{
	// Pre-draw, invalidate frame and depth buffers
	InvalidateBuffers();

	// Only raster primitive is triangle
	const uint64_t numTris = prim->getIndexBuffer().size() / 3;
	DBG_ASSERT((prim->getIndexBuffer().size() % 3) == 0);

	// Re-use VS attributes
	Vertex_IN in0, in1, in2;
	Vertex_OUT out0, out1, out2;

	for (uint64_t i = 0; i < numTris; i++)
	{
		vec3 v0, v1, v2;
		SetupTriangle(prim, i, v0, v1, v2);

		const vertex_shader VS = prim->VS();
		DBG_ASSERT(VS && "invalid vertex_shader!");

		UniformBuffer ubo = prim->UBO();

		in0.ResetData(); in1.ResetData(); in2.ResetData();
		out0.ResetData(); out1.ResetData(); out2.ResetData();
		FetchVertexAttributes(prim, i, in0, in1, in2);

		// Execute VS for each vertex
		const vec4 v0Clip = VS(v0, ubo, &in0, &out0);
		const vec4 v1Clip = VS(v1, ubo, &in1, &out1);
		const vec4 v2Clip = VS(v2, ubo, &in2, &out2);

		// Perspective-divide and convert to NDC
		const vec3 v0NDC = v0Clip / v0Clip.w;
		const vec3 v1NDC = v1Clip / v1Clip.w;
		const vec3 v2NDC = v2Clip / v2Clip.w;

		// Now to frame buffer-coordinates
		vec2 v0Raster = { (v0NDC.x + 1) / 2 * m_setup.viewport.width, (1 - v0NDC.y) / 2 * m_setup.viewport.height };
		vec2 v1Raster = { (v1NDC.x + 1) / 2 * m_setup.viewport.width, (1 - v1NDC.y) / 2 * m_setup.viewport.height };
		vec2 v2Raster = { (v2NDC.x + 1) / 2 * m_setup.viewport.width, (1 - v2NDC.y) / 2 * m_setup.viewport.height };

		const float triCoverage = PixelCoverage(v0Raster, v1Raster, v2Raster);
		if (prim->getPrimitiveSetup().cullMode == CullMode::CULL_BACK &&
			(!signbit(triCoverage) && m_setup.vertexWinding == VertexWinding::CLOCKWISE) ||
			(signbit(triCoverage) && m_setup.vertexWinding == VertexWinding::COUNTER_CLOCKWISE)) continue;

		Viewport vp;
		if (!clip2D(v0Raster, v1Raster, v2Raster, vp)) continue;

		// Re-use FS attributes
		Vertex_OUT FS_attribs;

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

					float z = ((w0 * v0NDC.z) + (w1 * v1NDC.z) + (w2 * v2NDC.z));
					if (z < m_depthBuffer[y * m_setup.viewport.width + x]) // Depth test; execute FS if passed & update z-buffer
					{
						m_depthBuffer[y * m_setup.viewport.width + x] = z;

						FS_attribs.ResetData(); // Reset attribs pre-FS for each fragment

						InterpolateAttributes(z, w0, w1, w2, out0, out1, out2, FS_attribs);
						const fragment_shader FS = prim->FS();
						const vec4 final_fragment = FS(ubo, &FS_attribs);

						m_frameBuffer[y * m_setup.viewport.width + x] = final_fragment;
					}
				}
			}
		}
	}
}