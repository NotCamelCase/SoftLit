#include "stdafx.h"

#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace glm;

namespace softlit
{
	Image::Image(const std::string & fileName)
	{
		m_data = stbi_load(fileName.c_str(), &m_width, &m_height, &m_numChannels, 0);
	}

	Image::~Image()
	{
		stbi_image_free(m_data);
		m_data = nullptr;
	}

	Texture::Texture(const Image& image)
		: m_image(image)
	{
	}

	Texture::~Texture()
	{
	}

	template<>
	glm::vec4 Texture::Sample<TextureSampler::SAMPLE_RGBA>(const glm::vec2& uv) const
	{
		DBG_ASSERT(m_image.m_numChannels == 4);

		//TODO: FILTER!!!
		uint32_t idxS = (uint32_t)glm::floor(uv.s * m_image.m_width);
		uint32_t idxT = (uint32_t)glm::floor(uv.t * m_image.m_height);
		uint32_t idx = (idxT * m_image.m_width + idxS) * m_image.m_numChannels;

		float g = (float)(m_image.m_data[idx++]);
		float b = (float)(m_image.m_data[idx++]);
		float r = (float)(m_image.m_data[idx++]);
		float a = (float)(m_image.m_data[idx++]);

		return glm::vec4(r, g, b, a) * g_texColDiv;
	}

	template<>
	glm::vec3 Texture::Sample<TextureSampler::SAMPLE_RGB>(const glm::vec2& uv) const
	{
		DBG_ASSERT(m_image.m_numChannels == 3);

		//TODO: FILTER!!!
		uint32_t idxS = (uint32_t)glm::floor(uv.s * m_image.m_width);
		uint32_t idxT = (uint32_t)glm::floor(uv.t * m_image.m_height);
		uint32_t idx = (idxT * m_image.m_width + idxS) * m_image.m_numChannels;

		float r = (float)(m_image.m_data[idx++]);
		float g = (float)(m_image.m_data[idx++]);
		float b = (float)(m_image.m_data[idx++]);

		return glm::vec3(r, g, b) * g_texColDiv;
	}
}