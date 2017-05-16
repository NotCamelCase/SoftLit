#pragma once

namespace softlit
{
	static constexpr float g_texColDiv = (1 / 255.f);

	class Texture;

	class Image
	{
	public:
		Image(const std::string& fileName);
		Image(uchar* data);
		~Image();

		friend class Texture;

	private:
		uchar* m_data = nullptr; // Image data

		int32_t m_width = 0;
		int32_t m_height = 0;
		int32_t m_numChannels = 0;
	};

	enum TextureSampler
	{
		SAMPLE_RGB = 3,
		SAMPLE_RGBA = 4
	};

	/* 2D Texture */
	class Texture
	{
	public:
		Texture(Image* image);
		~Texture();

		//TODO: Filter & address mode
		/* Function template to sample texture with 3 or 4 components i.e RGB or RGBA */
		template<TextureSampler N>
		glm::vec<N, float> Sample(const glm::vec2& uv)
		{
			static_assert((N <= 2) || (N > 4));
		}

		template<>
		glm::vec4 Sample(const glm::vec2& uv)
		{
			DBG_ASSERT(m_image->m_numChannels == 4);

			//TODO: FILTER!!!
			uint32_t idxS = (uint32_t)glm::floor(uv.s * m_image->m_width);
			uint32_t idxT = (uint32_t)glm::floor(uv.t * m_image->m_height);
			int64_t idx = (idxS * m_image->m_width + idxT) * m_image->m_numChannels;

			float r = (float)(m_image->m_data[idx++] * g_texColDiv);
			float g = (float)(m_image->m_data[idx++] * g_texColDiv);
			float b = (float)(m_image->m_data[idx++] * g_texColDiv);
			float a = (float)(m_image->m_data[idx++] * g_texColDiv);

			return glm::vec4(r, g, b, a);
		}

		template<>
		glm::vec3 Sample(const glm::vec2& uv)
		{
			DBG_ASSERT(m_image->m_numChannels == 3);

			//TODO: FILTER!!!
			uint32_t idxS = (uint32_t)glm::floor(uv.s * m_image->m_width);
			uint32_t idxT = (uint32_t)glm::floor(uv.t * m_image->m_height);
			int64_t idx = (idxS * m_image->m_width + idxT) * m_image->m_numChannels;

			float r = (float)(m_image->m_data[idx++] * g_texColDiv);
			float g = (float)(m_image->m_data[idx++] * g_texColDiv);
			float b = (float)(m_image->m_data[idx++] * g_texColDiv);

			return glm::vec3(r, g, b);
		}

	private:
		Image* m_image = nullptr;
	};
}