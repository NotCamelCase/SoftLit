#include "stdafx.h"

#include "Primitive.h"

using namespace std;
using namespace glm;
using namespace softlit;

Primitive::Primitive(vertex_shader vs, fragment_shader fs, const PrimitiveTopology top)
	: m_topology(top), m_VS(vs), m_FS(fs)
{
}

Primitive::~Primitive()
{
	m_vertexBuffer.clear();
	m_indexBuffer.clear();
}