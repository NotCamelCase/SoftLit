#pragma once

namespace softlit
{
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

	/* 2D Texture */
	class Texture
	{
	public:
		Texture(Image* image);
		~Texture();

		//TODO: Sample textures with different # of channels
		//TODO: Filter & address mode
		glm::vec4 Sample(const glm::vec2& uv);

	private:
		Image* m_image = nullptr;
	};
}
