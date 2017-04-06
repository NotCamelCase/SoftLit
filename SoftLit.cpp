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
	float fov;

	uint32_t width;
	uint32_t height;
};

int main(int argc, char* argv[])
{
	//TODO: Init rasterizer, set up default render settings (e.g. cull mode, viewport, winding order, etc.)

	RenderSettings rs;
	rs.fov = 45.f;
	rs.width = WIDTH;
	rs.height = HEIGHT;

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
	rasterSetup.cullMode = RasterizerSetup::CullMode::CULL_BACK;
	rasterSetup.vertexWinding = RasterizerSetup::VertexWinding::CLOCKWISE;
	rasterSetup.viewport = { 0.f, 0.f, rs.width, rs.height };

	Rasterizer* rasterizer = new Rasterizer(rasterSetup);

	const vec3 eye(6, 8, -10);
	const vec3 lookat(0, 0, -1);
	const vec3 up(0, 1, 0);

	mat4 view = lookAt(eye, lookat, up);
	mat4 proj = perspective(rs.fov, (float)rs.width / (float)rs.height, 0.1f, 100.f);

	SDL_Event event;
	bool running = true;
	while (running)
	{
		const auto drawBegin = chrono::high_resolution_clock::now();
		rasterizer->Draw(vertices, view, proj);
		const auto drawEnd = chrono::high_resolution_clock::now();

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

		const auto presentBegin = chrono::high_resolution_clock::now();
		display.Present();
		const auto presentEnd = chrono::high_resolution_clock::now();

		printf("Frame time: %lld\n", chrono::duration_cast<chrono::milliseconds> (presentEnd - drawBegin).count());
	}

	SAFE_DELETE(rasterizer);

	return 0;
}