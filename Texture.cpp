#include "stdafx.h"

#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace glm;
using namespace softlit;

static constexpr float s_texColDiv = (1 / 255.f);

Image::Image(const std::string & fileName)
{
	m_data = stbi_load(fileName.c_str(), &m_width, &m_height, &m_numChannels, 0);
}

Image::Image(uchar* data)
	: m_data(data)
{
}

Image::~Image()
{
	stbi_image_free(m_data);
	m_data = nullptr;
}

Texture::Texture(Image* image)
	: m_image(image)
{
}

Texture::~Texture()
{
}

vec4 Texture::Sample(const glm::vec2 & uv)
{
	//TODO: FILTER!!!
	uint32_t idxS = (uint32_t)(uv.s * m_image->m_width);
	uint32_t idxT = (uint32_t)(uv.t * m_image->m_height);
	int64_t idx = (idxS * m_image->m_width + idxT) * 4;

	float r = (float)(m_image->m_data[idx++] * s_texColDiv);
	float g = (float)(m_image->m_data[idx++] * s_texColDiv);
	float b = (float)(m_image->m_data[idx++] * s_texColDiv);
	float a = (float)(m_image->m_data[idx++] * s_texColDiv);

	return vec4(r, g, b, 1);
}