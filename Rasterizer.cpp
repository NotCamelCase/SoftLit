#include "stdafx.h"

#include "Rasterizer.h"

using namespace glm;

using std::vector;

namespace softlit
{
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
    inline float Rasterizer::PixelCoverage(const vec2& a, const vec2& b, const vec2& c) const
    {
        const int winding = (m_setup.vertexWinding == VertexWinding::COUNTER_CLOCKWISE) ? 1 : -1;
        const float x = (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);

        return winding * x;
    }

    void Rasterizer::SetupTriangle(Primitive* prim, const int64_t idx, glm::vec3& v0, glm::vec3& v1, glm::vec3& v2) const
    {
        const VertexBuffer& vbo = prim->getVertexBuffer();
        const IndexBuffer& ibo = prim->getIndexBuffer();

        v0 = vbo[ibo[idx * 3]];
        v1 = vbo[ibo[idx * 3 + 1]];
        v2 = vbo[ibo[idx * 3 + 2]];
    }

    bool softlit::Rasterizer::Clip2D(const glm::vec2 & v0, const glm::vec2 & v1, const glm::vec2 & v2, Viewport& vp) const
    {
        float xmin = min(v0.x, min(v1.x, v2.x));
        float xmax = max(v0.x, max(v1.x, v2.x));
        float ymin = min(v0.y, min(v1.y, v2.y));
        float ymax = max(v0.y, max(v1.y, v2.y));

        // Early-out when viewport bounds exceeded
        if (xmin + 1 > m_setup.viewport.width || xmax < 0 || ymin + 1 > m_setup.viewport.height || ymax < 0) return false;

        vp.x = max<uint32_t>(0, (int32_t)xmin);
        vp.width = min<uint32_t>(m_setup.viewport.width - 1, (int32_t)xmax);
        vp.y = max<uint32_t>(0, (int32_t)ymin);
        vp.height = min<uint32_t>(m_setup.viewport.height - 1, (int32_t)ymax);

        return true;
    }

    void Rasterizer::InterpolateAttributes(const float u, const float v, const float w, const Vertex_OUT& out0, const Vertex_OUT& out1, const Vertex_OUT& out2, Vertex_OUT& attribs) const
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

                vec4 attrib = u * attr0 + v * attr1 + w * attr2;

                attribs.PushVertexAttribute(attrib);
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

                vec3 attrib = u * attr0 + v * attr1 + w * attr2;

                attribs.PushVertexAttribute(attrib);
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

                vec2 attrib = u * attr0 + v * attr1 + w * attr2;

                attribs.PushVertexAttribute(attrib);
            }
        }
    }

    void Rasterizer::FetchVertexAttributes(const VertexAttributes& attribs, const int64_t idx, Vertex_IN& in0, Vertex_IN& in1, Vertex_IN& in2) const
    {
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
                in2.PushVertexAttribute(attribs.attrib_vec3[i][idx * 3 + 2]);
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
        // Only raster primitive is triangle
        const int64_t numTris = prim->getIndexBuffer().size() / 3;
        DBG_ASSERT((prim->getIndexBuffer().size() % 3) == 0);

        // Re-use VS attributes per primitive
        Vertex_IN in0, in1, in2;
        Vertex_OUT out0, out1, out2;

        // Re-use FS attributes per primitive
        Vertex_OUT FS_attribs;

#pragma omp parallel for private(in0, in1, in2, out0, out1, out2, FS_attribs)
        for (int64_t i = 0; i < numTris; i++)
        {
            vec3 v0, v1, v2;
            SetupTriangle(prim, i, v0, v1, v2);

            const vertex_shader VS = prim->getVS();
            DBG_ASSERT(VS && "invalid vertex_shader!");

            UniformBuffer ubo = prim->getUBO();

            in0.ResetData(); in1.ResetData(); in2.ResetData();
            out0.ResetData(); out1.ResetData(); out2.ResetData();
            FetchVertexAttributes(prim->getVertexAttributes(), i, in0, in1, in2);

            // Execute VS for each vertex
            const vec4 v0Clip = VS(v0, ubo, &in0, &out0);
            const vec4 v1Clip = VS(v1, ubo, &in1, &out1);
            const vec4 v2Clip = VS(v2, ubo, &in2, &out2);

            const float oneOverW0 = 1.f / v0Clip.w;
            const float oneOverW1 = 1.f / v1Clip.w;
            const float oneOverW2 = 1.f / v2Clip.w;

            // Perspective-divide and convert to NDC
            const vec3 v0NDC = v0Clip * oneOverW0;
            const vec3 v1NDC = v1Clip * oneOverW1;
            const vec3 v2NDC = v2Clip * oneOverW2;

            // Now to frame buffer-coordinates
            vec2 v0Raster = { (v0NDC.x + 1) / 2 * m_setup.viewport.width, (1 - v0NDC.y) / 2 * m_setup.viewport.height };
            vec2 v1Raster = { (v1NDC.x + 1) / 2 * m_setup.viewport.width, (1 - v1NDC.y) / 2 * m_setup.viewport.height };
            vec2 v2Raster = { (v2NDC.x + 1) / 2 * m_setup.viewport.width, (1 - v2NDC.y) / 2 * m_setup.viewport.height };

            const float triCoverage = PixelCoverage(v0Raster, v1Raster, v2Raster);
            if ((prim->getPrimitiveSetup().cullMode == CullMode::CULL_BACK) &&
                (triCoverage > 0 && m_setup.vertexWinding == VertexWinding::CLOCKWISE) ||
                (triCoverage < 0 && m_setup.vertexWinding == VertexWinding::COUNTER_CLOCKWISE)) continue;

            Viewport vp;
            if (!Clip2D(v0Raster, v1Raster, v2Raster, vp)) continue;

            // Store edges
            const vec2 edge0 = v2 - v1;
            const vec2 edge1 = v0 - v2;
            const vec2 edge2 = v1 - v0;

            // Pre-compute a flag for each edge to check for shared vertices and only include bottom/left edges
            const bool t0 = (edge0.x != 0) ? (edge0.x > 0) : (edge0.y > 0);
            const bool t1 = (edge1.x != 0) ? (edge1.x > 0) : (edge1.y > 0);
            const bool t2 = (edge2.x != 0) ? (edge2.x > 0) : (edge2.y > 0);

            // Triangle traversal
            for (uint32_t y = vp.y; y <= vp.height; y++)
            {
                for (uint32_t x = vp.x; x <= vp.width; x++)
                {
                    vec2 sample = { x + 0.5f, y + 0.5f };

                    // Evaluate edge functions on sample
                    float e0 = PixelCoverage(v1Raster, v2Raster, sample);
                    float e1 = PixelCoverage(v2Raster, v0Raster, sample);
                    float e2 = PixelCoverage(v0Raster, v1Raster, sample);

                    bool included = true;
                    included &= (e0 == 0) ? t0 : (e0 > 0);
                    included &= (e1 == 0) ? t1 : (e1 > 0);
                    included &= (e2 == 0) ? t2 : (e2 > 0);

                    if (included)
                    {
                        e0 /= triCoverage;
                        e1 /= triCoverage;
                        e2 = 1.f - e0 - e1;

                        // Interpolate depth
                        float z = (e0 * v0NDC.z) + (e1 * v1NDC.z) + (e2 * v2NDC.z);

                        if (z < m_depthBuffer[y * m_setup.viewport.width + x]) // Depth test; execute FS if passed & update z-buffer
                        {
                            m_depthBuffer[y * m_setup.viewport.width + x] = z;

                            FS_attribs.ResetData(); // Reset attribs pre-FS for each fragment

                            const float f0 = e0 * oneOverW0;
                            const float f1 = e1 * oneOverW1;
                            const float f2 = e2 * oneOverW2;

                            // Calc barycentric coordinates for perspectively-correct interpolation
                            const float u = f0 / (f0 + f1 + f2);
                            const float v = f1 / (f0 + f1 + f2);
                            const float w = 1.f - u - v;

                            InterpolateAttributes(u, v, w, out0, out1, out2, FS_attribs);
                            const fragment_shader FS = prim->getFS();
                            const vec4 final_fragment = FS(ubo, &FS_attribs, prim->getTBO());

                            m_frameBuffer[y * m_setup.viewport.width + x] = final_fragment;
                        }
                    }
                }
            }
        }
    }
}