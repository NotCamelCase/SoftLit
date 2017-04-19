#pragma once

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
		CLOCKWISE,
		COUNTER_CLOCKWISE // Default
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

		const std::vector<glm::vec4>& getFrameBuffer() const
		{
			return m_frameBuffer;
		}

		const std::vector<float>& getZBuffer() const
		{
			return m_depthBuffer;
		}

		/*
		* Rasterize given primitive set
		*/
		void Draw(const std::vector<glm::vec3>& vertices, const glm::mat4& view, const glm::mat4& proj);

	private:
		std::vector<glm::vec4> m_frameBuffer; // Used to hold rasterized primitives color buffer, colors in [0.f, 1.f]
		std::vector<float> m_depthBuffer; // Used as depth buffer

		RasterizerSetup m_setup;

		float PixelCoverage(const glm::vec3& a, const glm::vec3& b, const glm::vec2& c);

		void InvalidateBuffers()
		{
			fill(m_frameBuffer.begin(), m_frameBuffer.end(), glm::vec4(1, 1, 1, 1));
			fill(m_depthBuffer.begin(), m_depthBuffer.end(), FLT_MAX);
		}
	};
}