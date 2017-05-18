#pragma once

namespace softlit
{
	static constexpr float g_texColDiv = (1 / 255.f);

	class Texture;

	class Image
	{
	public:
		Image(const std::string& fileName);
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
		Texture(const Image& image);
		~Texture();

		//TODO: Filter & address modes

		template<TextureSampler T>
		glm::vec<T, float> Sample(const glm::vec2& uv) const;

	private:
		const Image& m_image;
	};
}