#include "stdafx.h"

#define WIDTH 1024
#define HEIGHT 768

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);
	atexit(SDL_Quit);

	SDL_Window* window = SDL_CreateWindow("SoftLit", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_RendererInfo info;
	SDL_GetRendererInfo(renderer, &info);
	std::cout << "Renderer name: " << info.name << std::endl;
	std::cout << "Texture formats: " << std::endl;
	for (Uint32 i = 0; i < info.num_texture_formats; i++)
	{
		std::cout << SDL_GetPixelFormatName(info.texture_formats[i]) << std::endl;
	}

	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING | SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);

	std::vector<unsigned char> colorBuffer(WIDTH * HEIGHT * 4, 0);

	SDL_Event event;
	bool running = true;
	while (running)
	{
		const Uint64 start = SDL_GetPerformanceCounter();

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);

		while (SDL_PollEvent(&event))
		{
			if ((SDL_QUIT == event.type) ||
				(SDL_KEYDOWN == event.type && SDL_SCANCODE_ESCAPE == event.key.keysym.scancode))
			{
				running = false;
				break;
			}
		}

		for (uint32_t y = 0; y < WIDTH; y++)
		{
			for (uint32_t x = 0; x < HEIGHT; x++)
			{
				//TODO: Present rendered frame
			}
		}

		SDL_UpdateTexture(texture, nullptr, colorBuffer.data(), WIDTH * 4);

		SDL_RenderCopy(renderer, texture, nullptr, nullptr);
		SDL_RenderPresent(renderer);

		const Uint64 end = SDL_GetPerformanceCounter();
		const static Uint64 freq = SDL_GetPerformanceFrequency();
		const double seconds = (end - start) / static_cast<double>(freq);
		std::cout << "Frame time: " << seconds * 1000.0 << "ms" << std::endl;
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}