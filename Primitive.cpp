#include "stdafx.h"

#include "Primitive.h"

using namespace std;
using namespace glm;
using namespace softlit;

Primitive::Primitive(const PrimitiveTopology top)
	: m_topology(top)
{
}

Primitive::~Primitive()
{
	m_vertexBuffer.clear();
	m_indexBuffer.clear();
}