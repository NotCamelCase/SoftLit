#include "stdafx.h"

#include "Display.h"

#include <SDL_render.h>

using namespace std;
using namespace softlit;

Display::Display(const uint32_t w, const uint32_t h)
	: m_width(w), m_height(h)
{
	SDL_Init(SDL_INIT_EVERYTHING);

	m_window = SDL_CreateWindow("SoftLit", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_width, m_height, SDL_WINDOW_SHOWN);

	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);

	SDL_RendererInfo info;
	SDL_GetRendererInfo(m_renderer, &info);
	printf("Renderer name: %s\n", info.name);
	printf("Texture formats: \n");
	for (uint32_t i = 0; i < info.num_texture_formats; i++)
	{
		printf("%s\n", SDL_GetPixelFormatName(info.texture_formats[i]));
	}

	m_renderTexture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, m_width, m_height);

	m_frameBuffer.resize(m_width * m_height * 4);
}

Display::~Display()
{
	SDL_DestroyTexture(m_renderTexture);
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	SDL_Quit();

	m_frameBuffer.clear();
}

void Display::ClearSurface()
{
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(m_renderer);
}

void Display::Present()
{
	SDL_UpdateTexture(m_renderTexture, nullptr, m_frameBuffer.data(), m_width * 4);

	SDL_RenderCopy(m_renderer, m_renderTexture, nullptr, nullptr);
	SDL_RenderPresent(m_renderer);
}

void Display::UpdateFrameBuffer(const vector<vec4>& colorBuffer)
{
	for (uint32_t i = 0; i < m_width * m_height * 4; i += 4)
	{
		const int pitch = i / 4;
		const vec3& rgba = colorBuffer[pitch];
		assert(length2(rgba) == 0);
		m_frameBuffer[i] = rgba.r;
		m_frameBuffer[i + 1] = rgba.g;
		m_frameBuffer[i + 2] = rgba.b;
		m_frameBuffer[i + 3] = SDL_ALPHA_OPAQUE;
	}
}