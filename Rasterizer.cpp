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

void Rasterizer::setupTriangle(Primitive* prim, const uint64_t idx, glm::vec3& v0, glm::vec3& v1, glm::vec3& v2) const
{
	const VertexBuffer& vbo = prim->getVertexBuffer();
	const IndexBuffer& ibo = prim->getIndexBuffer();

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

void Rasterizer::interpolateAttributes(const float z, const float w0, const float w1, const float w2, const Vertex_OUT& out0, const Vertex_OUT& out1, const Vertex_OUT& out2, Vertex_OUT& attribs)
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

			float xInt = w0 * attr0.x + w1 * attr1.x + w2 * attr2.x;
			float yInt = w0 * attr0.y + w1 * attr1.y + w2 * attr2.y;
			float zInt = w0 * attr0.z + w1 * attr1.z + w2 * attr2.z;
			float wInt = w0 * attr0.w + w1 * attr1.w + w2 * attr2.w;

			attribs.pushVertexAttribute(vec4(xInt, yInt, zInt, wInt) * z);
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

			float xInt = w0 * attr0.x + w1 * attr1.x + w2 * attr2.x;
			float yInt = w0 * attr0.y + w1 * attr1.y + w2 * attr2.y;
			float zInt = w0 * attr0.z + w1 * attr1.z + w2 * attr2.z;

			attribs.pushVertexAttribute(vec3(xInt, yInt, zInt) * z);
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

			float xInt = w0 * attr0.x + w1 * attr1.x + w2 * attr2.x;
			float yInt = w0 * attr0.y + w1 * attr1.y + w2 * attr2.y;

			attribs.pushVertexAttribute(vec2(xInt, yInt) * z);
		}
	}
}

void Rasterizer::fetchVertexAttributes(Primitive* prim, uint64_t idx, Vertex_IN& in0, Vertex_IN& in1, Vertex_IN& in2)
{
	const VertexAttributes& attribs = prim->getVertexAttributes();

	// Fetch attributes of type vec4
	if (!attribs.attrib_vec4.empty()) // At least one vec4 attribute buffer created
	{
		for (int i = 0; i < attribs.attrib_vec4.size(); i++)
		{
			in0.pushVertexAttribute(attribs.attrib_vec4[i][idx]);
			in1.pushVertexAttribute(attribs.attrib_vec4[i][idx + 1]);
			in2.pushVertexAttribute(attribs.attrib_vec4[i][idx + 2]);
		}
	}

	// Fetch attributes of type vec3
	if (!attribs.attrib_vec3.empty()) // At least one vec4 attribute buffer created
	{
		for (int i = 0; i < attribs.attrib_vec3.size(); i++)
		{
			in0.pushVertexAttribute(attribs.attrib_vec3[i][idx]);
			in1.pushVertexAttribute(attribs.attrib_vec3[i][idx + 1]);
			in2.pushVertexAttribute(attribs.attrib_vec3[i][idx + 2]);
		}
	}

	// Fetch attributes of type vec2
	if (!attribs.attrib_vec2.empty()) // At least one vec4 attribute buffer created
	{
		for (int i = 0; i < attribs.attrib_vec2.size(); i++)
		{
			in0.pushVertexAttribute(attribs.attrib_vec2[i][idx]);
			in1.pushVertexAttribute(attribs.attrib_vec2[i][idx + 1]);
			in2.pushVertexAttribute(attribs.attrib_vec2[i][idx + 2]);
		}
	}
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

		Vertex_IN in0, in1, in2;
		Vertex_OUT out0, out1, out2;

		fetchVertexAttributes(prim, i, in0, in1, in2);

		// Execute VS for each vertex
		const vec4 v0Clip = VS(v0, ubo, &in0, &out0);
		const vec4 v1Clip = VS(v1, ubo, &in1, &out1);
		const vec4 v2Clip = VS(v2, ubo, &in2, &out2);

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

					float z = 1.f / ((w0 * v0Raster.z) + (w1 * v1Raster.z) + (w2 * v2Raster.z));
					if (z < m_depthBuffer[y * m_setup.viewport.width + x]) // Depth test; execute FS if passed & update z-buffer
					{
						Vertex_OUT FS_attribs;
						interpolateAttributes(z, w0, w1, w2, out0, out1, out2, FS_attribs);
						const vec4 final_fragment = prim->FS()(ubo, &FS_attribs);

						m_frameBuffer[y * m_setup.viewport.width + x] = final_fragment;
						m_depthBuffer[y * m_setup.viewport.width + x] = z;
					}
				}
			}
		}
	}
}