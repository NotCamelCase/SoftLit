#pragma once

struct SDL_Renderer;
struct SDL_Window;
struct SDL_Texture;

namespace softlit
{
	// SDL is the underlying display facility used to present frames
	// SDL_PIXELFORMAT_ARGB8888 is the pre-defined surface format
	class Display
	{
	public:
		Display(const uint32_t w, const uint32_t h, bool fullscreen);
		~Display();

		// Clear render surface before presenting back buffer content
		// clearColor [0, 255]
		void ClearRenderTarget(const glm::ivec3& clearColor);

		// Render frame buffer to SDL render target
		void Present();

		// Update display color buffer with frame buffer content before presenting
		void UpdateColorBuffer(const std::vector<glm::vec4>& frameBuffer);

	private:
		SDL_Renderer* m_renderer = nullptr;
		SDL_Window* m_window = nullptr;
		SDL_Texture* m_renderTexture = nullptr;

		// Color buffer that SDL will use to present frame buffer content
		std::vector<uchar> m_colorBuffer;
		uint32_t m_width = 1024u;
		uint32_t m_height = 768u;
	};
}