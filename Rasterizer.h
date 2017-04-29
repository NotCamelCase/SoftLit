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
		const DepthBuffer& getZBuffer() const { return m_depthBuffer; }

		/*
		* Rasterize given primitive set
		*/
		void Draw(Primitive* prim);

	private:
		FrameBuffer m_frameBuffer; // Used to hold rasterized primitives color buffer, colors in [0.f, 1.f]
		DepthBuffer m_depthBuffer; // Used as depth buffer

		RasterizerSetup m_setup;

		float PixelCoverage(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c);

		void InvalidateBuffers()
		{
			fill(m_frameBuffer.begin(), m_frameBuffer.end(), glm::vec4(1, 1, 1, 1));
			fill(m_depthBuffer.begin(), m_depthBuffer.end(), FLT_MAX);
		}

		/*
		* Set ups a triangle based on index into the index buffer of primitive and triangle topology e.g TRIANGLE_LIST, TRIANGLE_STRIP
		*/
		void SetupTriangle(Primitive* primitive, const uint64_t idx, glm::vec3& v0, glm::vec3& v1, glm::vec3& v2) const;

		// Apply 2D viewport clipping
		bool clip2D(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, Viewport&) const;

		/* Generates and fills in perspectively-correct interpolated barycentric fragment shader attributes to be passed */
		void InterpolateAttributes(const float z, const float w0, const float w1, const float w2, const Vertex_OUT& out0, const Vertex_OUT& out1, const Vertex_OUT& out2, Vertex_OUT& attribs);

		/* Fetch vertex attributes from primitive's attribute buffer and fill in corresponding vertex attributes */
		void FetchVertexAttributes(Primitive* prim, uint64_t idx, Vertex_IN& in0, Vertex_IN& in1, Vertex_IN& in2);
	};
}