#include "stdafx.h"

#include "Primitive.h"

#include "Texture.h"

using namespace glm;

namespace softlit
{
	Primitive::Primitive(const PrimitiveSetup& setup)
		: m_setup(setup)
	{
	}

	Primitive::~Primitive()
	{
		m_vertexBuffer.clear();
		m_indexBuffer.clear();

		for (Texture* t : m_tbo)
			SAFE_DELETE(t);
		m_tbo.clear();
	}
}