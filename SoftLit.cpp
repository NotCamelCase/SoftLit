#include "stdafx.h"

#include "Rasterizer.h"
#include "Primitive.h"
#include "Display.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define WIDTH 1280
#define HEIGHT 720

using namespace std;
using namespace glm;
using namespace softlit;

//#define SINGLE_FRAME_OUTPUT

struct mat_ubo
{
	mat4 MVP;
};

// VS
vec4 VS_Simple(const glm::vec3& pos, mat_ubo* ubo)
{
	return (ubo->MVP * vec4(pos, 1));
}

// FS
vec4 FS_Simple()
{
	return vec4(0.25, 0.5, 0.75, 1.);
}

void ImportScene(vector<Primitive*>& objects, const string&);

int main(int argc, char* argv[])
{
	float fov = 60.f;
	uint32_t width = WIDTH;
	uint32_t height = HEIGHT;

	// Init SDL
	Display display(width, height, false);

	vector<Primitive*> objects;

	ImportScene(objects, "../cube.obj");

	DBG_ASSERT(!objects.empty() && "Failed to import models!");

	RasterizerSetup rasterSetup;
	rasterSetup.cullMode = CullMode::CULL_DISABLED;
	rasterSetup.vertexWinding = VertexWinding::CLOCKWISE;
	rasterSetup.viewport = { 0u, 0u, width, height };

	Rasterizer* rasterizer = new Rasterizer(rasterSetup);

	vec3 eye(3, 4, -5);
	vec3 lookat(0, 0, 0);
	vec3 up(0, 1, 0);

	mat4 view = lookAtRH(eye, lookat, up);
	mat4 proj = perspectiveRH(glm::radians(fov), (float)width / (float)height, 0.5f, 100.f);
	mat4 model;

	// Create primitive shading data
	const auto VS = reinterpret_cast<vertex_shader> (&VS_Simple);
	const auto FS = reinterpret_cast<fragment_shader> (&FS_Simple);

	mat_ubo ubo;

	for (Primitive* prim : objects)
	{
		prim->VS(VS);
		prim->FS(FS);
		prim->UBO(&ubo);
	}

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

		display.ClearRenderTarget(ivec3(0, 0, 0));

		const auto drawBegin = chrono::high_resolution_clock::now();
		for (Primitive* prim : objects)
		{
			model = rotate(model, 0.025f, vec3(0, 1, 0));
			mat4 mvp = proj * view * model;

			mat_ubo* ubo = static_cast<mat_ubo*> (prim->UBO());
			ubo->MVP = mvp;

			rasterizer->Draw(prim, view, proj);
		}
		const auto drawEnd = chrono::high_resolution_clock::now();
		//printf("Draw time: %lld\n", chrono::duration_cast<chrono::milliseconds> (drawEnd - drawBegin).count());

		display.UpdateColorBuffer(rasterizer->getFrameBuffer());

		const auto presentBegin = chrono::high_resolution_clock::now();
		display.Present();
		const auto presentEnd = chrono::high_resolution_clock::now();

		printf("Frame time: %lld\n", chrono::duration_cast<chrono::milliseconds> (presentEnd - drawBegin).count());
	}
#endif

	for (Primitive* obj : objects)
		SAFE_DELETE(obj);

	SAFE_DELETE(rasterizer);

	return 0;
}

void ImportScene(vector<Primitive*>& objs, const string& filename)
{
	tinyobj::attrib_t attribs;
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;

	//TODO: Handle multiple objects in a single .obj

	string err;
	bool ret = tinyobj::LoadObj(&attribs, &shapes, &materials, &err, filename.c_str());
	if (ret)
	{
		for (size_t i = 0; i < shapes.size(); i++)
		{
			const tinyobj::shape_t& shape = shapes[i];

			VertexBuffer vbo;
			IndexBuffer ibo;

			const vector<tinyobj::index_t> indexBuffer = shape.mesh.indices;

			Primitive* obj = new Primitive(PrimitiveTopology::TRIANGLE_LIST);
			objs.push_back(obj);

			for (size_t idx = 0; idx < indexBuffer.size(); idx++)
			{
				DBG_ASSERT(indexBuffer[idx].vertex_index != -1);
				const uint64_t vtxIndex = indexBuffer[idx].vertex_index;

				//TODO: Handle other vertex attributes

				ibo.push_back(vtxIndex);
			}

			for (size_t f = 0; f < attribs.vertices.size(); f += 3) // 3 -> # of vertices in a triangle face
			{
				const float p0 = attribs.vertices[f];
				const float p1 = attribs.vertices[f + 1];
				const float p2 = attribs.vertices[f + 2];

				vbo.push_back(vec3(p0, p1, p2));
			}

			obj->setVertexBuffer(vbo);
			obj->setIndexBuffer(ibo);
		}
	}
}