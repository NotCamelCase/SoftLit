#include "stdafx.h"

#include "Rasterizer.h"
#include "Primitive.h"
#include "Shaders.h"
#include "Display.h"
#include "Texture.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define WIDTH 1280
#define HEIGHT 720
#define DISPLAY_FULLSCREEN false

using namespace glm;
using namespace softlit;

using std::vector;

//#define SINGLE_FRAME_OUTPUT

void ImportOBJ(vector<Primitive*>& objects, const string&);

int main(int argc, char* argv[])
{
	float fov = 60.f;
	uint32_t width = WIDTH;
	uint32_t height = HEIGHT;

	vector<Primitive*> objects;
	vector<Image*> images;

	//TODO: Handle multiple objects in a single .obj

	ImportOBJ(objects, "../assets/chalet.obj");

	DBG_ASSERT(!objects.empty() && "Failed to import models!");

	Image* img = new Image("../assets/chalet.jpg");
	DBG_ASSERT(img);

	RasterizerSetup rasterSetup;
	rasterSetup.vertexWinding = VertexWinding::COUNTER_CLOCKWISE;
	rasterSetup.viewport = { 0u, 0u, width, height };

	Rasterizer* rasterizer = new Rasterizer(rasterSetup);

	vec3 eye(0, 0, -5);
	vec3 lookat(0, 0, 0);
	vec3 up(0, 1, 0);

	mat4 view = lookAt(eye, lookat, up);
	mat4 proj = perspective(glm::radians(fov), (float)width / (float)height, 0.5f, 100.f);
	vector<mat4> models(objects.size());

	// Handles to shaders
	const auto VS = reinterpret_cast<vertex_shader> (&VS_Simple);
	const auto FS = reinterpret_cast<fragment_shader> (&FS_Simple);

	const auto VS_textured = reinterpret_cast<vertex_shader> (&VS_Textured);
	const auto FS_textured = reinterpret_cast<fragment_shader> (&FS_Textured);

	UBO ubo;

	for (int i = 0; i < objects.size(); i++)
	{
		Primitive* prim = objects[i];
		prim->setVS(VS_textured);
		prim->setFS(FS_textured);
		prim->setUBO(&ubo);
		prim->addTexture(new Texture(*img));
	}

#ifdef SINGLE_FRAME_OUTPUT
	rasterizer->ClearBuffers();

	for (Primitive* prim : objects)
	{
		//model = rotate(model, 0.025f, vec3(0, 1, 0));
		mat4 mv = view * model;
		mat3 normal = { mv[0], mv[1], mv[2] };
		mat4 mvp = proj * mv;

		mat_ubo* ubo = static_cast<mat_ubo*> (prim->UBO());
		ubo->MVP = mvp;
		ubo->NORMAL = normal;

		rasterizer->Draw(prim);
	}

	const vector<vec4>& frameBuffer = rasterizer->getFrameBuffer();
	FILE *f = NULL;
	fopen_s(&f, "../render.ppm", "w");
	fprintf(f, "P3\n%d %d\n%d\n ", WIDTH, HEIGHT, 255);
	for (int32_t i = 0; i < WIDTH * HEIGHT; ++i)
	{
		uint r = (uint)(255 * frameBuffer[i].x);
		uint g = (uint)(255 * frameBuffer[i].y);
		uint b = (uint)(255 * frameBuffer[i].z);
		fprintf(f, "%d %d %d ", r, g, b);
	}
	fclose(f);

	printf("DONE\n");

#else
	// Init SDL
	Display display(width, height, DISPLAY_FULLSCREEN);

	SDL_Event event;
	bool running = true;
	bool paused = false;
	while (running)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
				switch (event.key.keysym.scancode)
				{
				case SDL_SCANCODE_SPACE:
					paused = !paused;
					break;
				case SDL_SCANCODE_ESCAPE:
					running = false;
					break;
				default:
					break;
				}
				break;
			case SDL_KEYUP:
				break;
			case SDL_QUIT:
				running = false;
				break;
			default:
				break;
			}
		}

		const auto drawBegin = chrono::high_resolution_clock::now();
		if (!paused) // When unpaused, re-draw every frame
		{
			display.ClearRenderTarget(ivec3(0, 0, 0));

			// Pre-draw, invalidate frame and depth buffers
			rasterizer->ClearBuffers();

			for (size_t i = 0; i < objects.size(); i++)
			{
				Primitive* prim = objects[i];
				models[i] = rotate(models[i], 0.025f, vec3(0, 1, 0));
				mat4 mv = view * models[i];
				mat4 mvp = proj * mv;

				UBO* ubo = static_cast<UBO*> (prim->getUBO());
				ubo->MVP = mvp;
				ubo->MV = mv;

				rasterizer->Draw(prim);
			}
			const auto drawEnd = chrono::high_resolution_clock::now();
			//printf("Draw time: %lld\n", chrono::duration_cast<chrono::milliseconds> (drawEnd - drawBegin).count());

			display.UpdateColorBuffer(rasterizer->getFrameBuffer());
		}

		const auto presentBegin = chrono::high_resolution_clock::now();
		display.Present();
		const auto presentEnd = chrono::high_resolution_clock::now();
		//printf("Display time: %lld\n", chrono::duration_cast<chrono::milliseconds> (presentEnd - presentBegin).count());

		printf("Frame time: %lld\n", chrono::duration_cast<chrono::milliseconds> (presentEnd - drawBegin).count());
	}
#endif

	for (Primitive* obj : objects)
		SAFE_DELETE(obj);

	for (Image* img : images)
		SAFE_DELETE(img);

	SAFE_DELETE(rasterizer);

	return EXIT_SUCCESS;
}

void ImportOBJ(vector<Primitive*>& objs, const string& filename)
{
	tinyobj::attrib_t attribs;
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;

	string err;
	bool ret = tinyobj::LoadObj(&attribs, &shapes, &materials, &err, filename.c_str());
	if (ret)
	{
		const bool hasUV = !attribs.texcoords.empty();
		const bool hasNormals = !attribs.normals.empty();

		for (size_t i = 0; i < shapes.size(); i++)
		{
			const tinyobj::shape_t& shape = shapes[i];

			VertexBuffer vbo;
			IndexBuffer ibo;

			AttributeBuffer<3> normals;
			AttributeBuffer<2> uvs;

			const vector<tinyobj::index_t>& indexBuffer = shape.mesh.indices;

			PrimitiveSetup ps;
			ps.cullMode = CullMode::CULL_BACK;

			Primitive* obj = new Primitive(ps);
			objs.push_back(obj);

			for (size_t idx = 0; idx < indexBuffer.size(); idx++)
			{
				DBG_ASSERT(indexBuffer[idx].vertex_index != -1);
				const uint64_t vtxIndex = indexBuffer[idx].vertex_index;
				ibo.push_back(vtxIndex);

				if (hasNormals)
				{
					DBG_ASSERT(indexBuffer[idx].normal_index != -1);
					normals.m_index.push_back(indexBuffer[idx].normal_index);
				}

				if (hasUV)
				{
					DBG_ASSERT(indexBuffer[idx].texcoord_index != -1);
					uvs.m_index.push_back(indexBuffer[idx].texcoord_index);
				}
			}

			for (size_t f = 0; f < attribs.vertices.size(); f += 3) // 3 -> # of vertices in a triangle face
			{
				// object has to have vertices
				const float p0 = attribs.vertices[f];
				const float p1 = attribs.vertices[f + 1];
				const float p2 = attribs.vertices[f + 2];
				vbo.push_back(vec3(p0, p1, p2));
			}

			if (hasNormals)
			{
				for (size_t f = 0; f < attribs.normals.size(); f += 3)
				{
					const float n0 = attribs.normals[f];
					const float n1 = attribs.normals[f + 1];
					const float n2 = attribs.normals[f + 2];
					normals.m_data.push_back(vec3(n0, n1, n2));
				}
			}

			if (hasUV)
			{
				for (size_t f = 0; f < attribs.texcoords.size(); f += 2)
				{
					const float u = attribs.texcoords[f];
					const float v = 1.f - attribs.texcoords[f + 1]; // Flip v as .OBJ assumes bottom-left the origin in texture space
					uvs.m_data.push_back(vec2(u, v));
				}
			}

			obj->setVertexBuffer(vbo);
			obj->setIndexBuffer(ibo);
			if (hasUV) obj->appendAttributeBuffer(uvs);
			if (hasNormals) obj->appendAttributeBuffer(normals);
		}
	}
}