#include "stdafx.h"

#include "Rasterizer.h"
#include "Primitive.h"
#include "Display.h"

#define WIDTH 1280
#define HEIGHT 720

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

struct mat_ubo
{
	mat4 MVP;
};

vec4 VS_Simple(const glm::vec3& pos, mat_ubo* ubo)
{
	return (ubo->MVP * vec4(pos, 1));
}

vec4 FS_Simple()
{
	return vec4(0.25, 0.5, 0.75, 1.);
}

int main(int argc, char* argv[])
{
	RenderSettings rs;
	rs.fov = 60.f;
	rs.width = WIDTH;
	rs.height = HEIGHT;

	// Init SDL
	Display display(rs.width, rs.height, false);

	vector<vec3> vertices =
	{
		vec3{ 1.000000, -1.000000, -1.000000 },
		vec3{ 1.000000, -1.000000, 1.000000 },
		vec3{ -1.000000, -1.000000, 1.000000 },
		vec3{ -1.000000, -1.000000, -1.000000 },
		vec3{ 1.000000, 1.000000, -0.999999 },
		vec3{ 0.999999, 1.000000, 1.000001 },
		vec3{ -1.000000, 1.000000, 1.000000 },
		vec3{ -1.000000, 1.000000, -1.000000 }
	};

	vector<uint64_t> indices =
	{
		1,3,0,
		7,5,4,
		4,1,0,
		5,2,1,
		2,7,3,
		0,7,4,
		1,2,3,
		7,6,5,
		4,5,1,
		5,6,2,
		2,6,7,
		0,3,7,
	};

	RasterizerSetup rasterSetup;
	rasterSetup.cullMode = CullMode::CULL_DISABLED;
	rasterSetup.vertexWinding = VertexWinding::CLOCKWISE;
	rasterSetup.viewport = { 0.f, 0.f, rs.width, rs.height };

	Rasterizer* rasterizer = new Rasterizer(rasterSetup);

	vec3 eye(3, 4, -5);
	vec3 lookat(0, 0, 0);
	vec3 up(0, 1, 0);

	mat4 view = lookAtRH(eye, lookat, up);
	mat4 proj = perspectiveRH(glm::radians(rs.fov), (float)rs.width / (float)rs.height, 0.5f, 100.f);

	const auto VS = reinterpret_cast<vertex_shader> (&VS_Simple);
	const auto FS = reinterpret_cast<fragment_shader> (&FS_Simple);

	Primitive obj(VS, FS, PrimitiveTopology::TRIANGLE_LIST);
	obj.setVertexBuffer(vertices);
	obj.setIndexBuffer(indices);

	mat_ubo mvp_ubo;
	obj.UBO(static_cast<UniformBuffer> (&mvp_ubo));

	mat_ubo* ubo = static_cast<mat_ubo*> (obj.UBO());

#ifdef SINGLE_FRAME_OUTPUT
	rasterizer->Draw(vertices, indices, view, proj);

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

		mat4 cam = rotate(mat4(), 0.025f, vec3(1, 1, 0));
		view = view * cam;

		ubo->MVP = proj * view;

		display.ClearRenderTarget(ivec3(0, 0, 0));

		const auto drawBegin = chrono::high_resolution_clock::now();
		rasterizer->Draw(&obj, view, proj);
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