#include "stdafx.h"

#include "Display.h"

#include <SDL_render.h>

//TODO: REMOVE
#define WIDTH 1024
#define HEIGHT 768

using namespace softlit;

Display::Display()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window = SDL_CreateWindow("SoftLit", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_RendererInfo info;
	SDL_GetRendererInfo(renderer, &info);
	printf("Renderer name: %s\n", info.name);
	printf("Texture formats: \n");
	for (Uint32 i = 0; i < info.num_texture_formats; i++)
	{
		printf("%s\n", SDL_GetPixelFormatName(info.texture_formats[i]));;
	}

	m_renderTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING | SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);

	//TODO: Re-size frame buffer
}

Display::~Display()
{
	SDL_DestroyTexture(m_renderTexture);
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

void Display::ClearSurface()
{
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(m_renderer);
}

void Display::Present()
{
	SDL_UpdateTexture(m_renderTexture, nullptr, m_frameBuffer.data(), WIDTH * 4);

	SDL_RenderCopy(m_renderer, m_renderTexture, nullptr, nullptr);
	SDL_RenderPresent(m_renderer);
}