#pragma once

struct SDL_Renderer;
struct SDL_Window;
struct SDL_Texture;

namespace softlit
{
	// SDL is the underlying display facility used to present frames
	// SDL_PIXELFORMAT_ARGB8888 is the pre-define surface format
	class Display
	{
	public:
		Display();
		~Display();

		// Clear render surface to present frame buffer
		void ClearSurface();

		// Render a ready frame buffer
		void Present();

	private:
		SDL_Renderer* m_renderer = nullptr;
		SDL_Window* m_window = nullptr;
		SDL_Texture* m_renderTexture = nullptr;

		// Frame buffer that SDL will use to copy color buffer to OS Window
		std::vector<uchar> m_frameBuffer;
	};
}