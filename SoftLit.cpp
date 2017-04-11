#include "stdafx.h"

#include "MathUtils.h"

#include "Rasterizer.h"
#include "Display.h"

#define WIDTH 1024
#define HEIGHT 768

using namespace std;
using namespace softlit;

struct RenderSettings
{
	float fov = 60.f;

	uint32_t width = WIDTH;
	uint32_t height = HEIGHT;
};

int main(int argc, char* argv[])
{
	RenderSettings rs;
	rs.fov = 60.f;
	rs.width = WIDTH;
	rs.height = HEIGHT;

	// Init SDL
	Display display(rs.width, rs.height);

	vector<vec3> vertices =
	{
		vec3(-1, -1, 1),
		vec3(-1, 1, 1),
		vec3(1, -1, 1),
		vec3(1, -1, 1),
		vec3(-1, 1, 1),
		vec3(1, 1, 1),
		vec3(-1, -1, -1),
		vec3(1, -1, -1),
		vec3(-1, 1, -1),
		vec3(-1, 1, -1),
		vec3(1, -1, -1),
		vec3(1, 1, -1),
		vec3(-1, 1, 1),
		vec3(-1, -1, 1),
		vec3(-1, 1, -1),
		vec3(-1, 1, -1),
		vec3(-1, -1, 1),
		vec3(-1, -1, -1),
		vec3(1, 1, 1),
		vec3(1, 1, -1),
		vec3(1, -1, 1),
		vec3(1, -1, 1),
		vec3(1, 1, -1),
		vec3(1, -1, -1),
		vec3(1, 1, 1),
		vec3(-1, 1, 1),
		vec3(1, 1, -1),
		vec3(1, 1, -1),
		vec3(-1, 1, 1),
		vec3(-1, 1, -1),
		vec3(1, -1, 1),
		vec3(1, -1, -1),
		vec3(-1, -1, 1),
		vec3(-1, -1, 1),
		vec3(1, -1, -1),
		vec3(-1, -1, -1)
	};

	RasterizerSetup rasterSetup;
	rasterSetup.cullMode = CullMode::CULL_BACK;
	rasterSetup.vertexWinding = VertexWinding::CLOCKWISE;
	rasterSetup.viewport = { 0.f, 0.f, rs.width, rs.height };

	Rasterizer* rasterizer = new Rasterizer(rasterSetup);

	const vec3 eye(3, 4, 5);
	const vec3 lookat(0, 0, -10);
	const vec3 up(0, 1, 0);

	mat4 view = lookAt(eye, lookat, up);
	mat4 proj = perspective(rs.fov, (float)rs.width / (float)rs.height, 0.1f, 100.f);

	SDL_Event event;
	bool running = true;
	while (running)
	{
		while (SDL_PollEvent(&event))
		{
			if ((SDL_QUIT == event.type) ||
				(SDL_KEYDOWN == event.type && SDL_SCANCODE_ESCAPE == event.key.keysym.scancode))
			{
				running = false;
				break;
			}
		}

		display.ClearSurface();

		const auto drawBegin = chrono::high_resolution_clock::now();
		rasterizer->Draw(vertices, view, proj);
		const auto drawEnd = chrono::high_resolution_clock::now();

		display.UpdateFrameBuffer(rasterizer->getColorBuffer());

		const auto presentBegin = chrono::high_resolution_clock::now();
		display.Present();
		const auto presentEnd = chrono::high_resolution_clock::now();

		printf("Frame time: %lld\n", chrono::duration_cast<chrono::milliseconds> (presentEnd - drawBegin).count());
	}

	SAFE_DELETE(rasterizer);

	return 0;
}