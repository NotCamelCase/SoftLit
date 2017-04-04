#include "stdafx.h"

#include "Vector.h"
#include "Matrix.h"

#include "Display.h"

#define WIDTH 1024
#define HEIGHT 768

using namespace softlit;

int main(int argc, char* argv[])
{
	//TODO: Init rasterizer, set up default render settings (e.g. cull mode, viewport, winding order, etc.)

	Display display;

	SDL_Event event;
	bool running = true;
	while (running)
	{
		//TODO: Rasterize 3D primitives
		//TODO: Update SDL frame buffer via rasterized colorbuffer

		display.ClearSurface();

		while (SDL_PollEvent(&event))
		{
			if ((SDL_QUIT == event.type) ||
				(SDL_KEYDOWN == event.type && SDL_SCANCODE_ESCAPE == event.key.keysym.scancode))
			{
				running = false;
				break;
			}
		}

		display.Present();
	}

	return 0;
}