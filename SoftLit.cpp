#include "stdafx.h"

#include "Rasterizer.h"
#include "Display.h"

#define WIDTH 800
#define HEIGHT 600

using namespace std;
using namespace glm;
using namespace softlit;

struct RenderSettings
{
	float fov;

	uint32_t width;
	uint32_t height;
};

//#define SINGLE_FRAME_OUTPUT

int main(int argc, char* argv[])
{
	RenderSettings rs;
	rs.fov = 60.f;
	rs.width = WIDTH;
	rs.height = HEIGHT;

	// Init SDL
	Display display(rs.width, rs.height, false);

#if 1
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

#else
	vector<vec3> vertices =
	{
		vec3(-0.5, -0.5, 0.5),
		vec3(-0.5, 0.5, 0.5),
		vec3(0.5, -0.5, 0.5),
		/*vec3(0.5, -0.5, 0.5),
		vec3(-0.5, 0.5, 0.5),
		vec3(0.5, 0.5, 0.5)*/
	};
#endif

	RasterizerSetup rasterSetup;
	rasterSetup.cullMode = CullMode::CULL_BACK;
	rasterSetup.vertexWinding = VertexWinding::CLOCKWISE;
	rasterSetup.viewport = { 0.f, 0.f, rs.width, rs.height };

	Rasterizer* rasterizer = new Rasterizer(rasterSetup);

	vec3 eye(3, 4, -5);
	vec3 lookat(0, 0, 0);
	vec3 up(0, 1, 0);

	mat4 cam = mat4();
	mat4 view = lookAtRH(eye, lookat, up);
	mat4 proj = perspectiveRH(glm::radians(rs.fov), (float)rs.width / (float)rs.height, 0.5f, 100.f);

#ifdef SINGLE_FRAME_OUTPUT
	rasterizer->Draw(vertices, view, proj);

	const vector<vec4>& frameBuffer = rasterizer->getFrameBuffer();
	FILE *f = NULL;
	fopen_s(&f, "../render.ppm", "w");
	fprintf(f, "P3\n%d %d\n%d\n ", WIDTH, HEIGHT, 255);
	for (int32_t i = 0; i < WIDTH * HEIGHT; ++i)
	{
		uint r = (255 * frameBuffer[i].x);
		uint g = (255 * frameBuffer[i].y);
		uint b = (255 * frameBuffer[i].z);
		fprintf(f, "%d %d %d ", r, g, b);
	}
	fclose(f);

#else
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

		display.ClearRenderTarget(vec3(0, 0, 0));

		cam = glm::rotate(cam, 0.05f, vec3(1, 1, 0));
		view = lookAtRH(eye, lookat, up);
		view = view * cam;

		const auto drawBegin = chrono::high_resolution_clock::now();
		rasterizer->Draw(vertices, view, proj);
		const auto drawEnd = chrono::high_resolution_clock::now();

		display.UpdateColorBuffer(rasterizer->getFrameBuffer());

		const auto presentBegin = chrono::high_resolution_clock::now();
		display.Present();
		const auto presentEnd = chrono::high_resolution_clock::now();

		printf("Frame time: %lld\n", chrono::duration_cast<chrono::milliseconds> (presentEnd - drawBegin).count());
	}
#endif

	SAFE_DELETE(rasterizer);

	return 0;
}