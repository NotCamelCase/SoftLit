#pragma once

#include "Primitive.h"

namespace softlit
{
    typedef std::vector<glm::vec4> FrameBuffer;
    typedef std::vector<float> DepthBuffer;

    class Rasterizer
    {
    public:
        Rasterizer(const RasterizerSetup& setup);
        ~Rasterizer();

        const FrameBuffer& getFrameBuffer() const { return m_frameBuffer; }

        /* Rasterize a given 3-D primitive object by transforming its vertices to clip-space via VS, depth testing and calculating fragment output color via FS */
        void Draw(Primitive* prim);

        /* Clear depth and frame buffers pre-draw */
        void ClearBuffers(const glm::vec4& clearColor, const float depthValue = FLT_MAX)
        {
            fill(m_frameBuffer.begin(), m_frameBuffer.end(), clearColor);
            fill(m_depthBuffer.begin(), m_depthBuffer.end(), depthValue);
        }

    private:
        FrameBuffer m_frameBuffer; // Used to hold rasterized primitives color buffer, colors in [0.f, 1.f]
        DepthBuffer m_depthBuffer; // Used as depth buffer

        RasterizerSetup m_setup;

        /* Edge function to check whether pixel is covered by triangle or not */
        float PixelCoverage(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c) const;

        /* Set ups a triangle based on index into the index buffer of primitive and triangle topology e.g TRIANGLE_LIST, TRIANGLE_STRIP */
        void SetupTriangle(Primitive* primitive, const int64_t idx, glm::vec3& v0, glm::vec3& v1, glm::vec3& v2) const;

        // Apply 2D viewport clipping
        bool Clip2D(const glm::vec2& v0, const glm::vec2& v1, const glm::vec2& v2, Viewport&) const;

        /* Generates and fills in perspectively-correct interpolated barycentric fragment shader attributes to be passed */
        void InterpolateAttributes(const float u, const float v, const Vertex_OUT& out0, const Vertex_OUT& out1, const Vertex_OUT& out2, Vertex_OUT& attribs) const;

        /* Fetch vertex attributes from primitive's attribute buffer and fill in corresponding vertex attributes */
        void FetchVertexAttributes(const VertexAttributes& attribs, const int64_t idx, Vertex_IN& in0, Vertex_IN& in1, Vertex_IN& in2) const;
    };
}