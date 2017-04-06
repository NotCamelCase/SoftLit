#include "stdafx.h"

#include "Rasterizer.h"

using namespace std;
using namespace softlit;

Rasterizer::Rasterizer(const RasterizerSetup& setup)
	: m_setup(setup)
{
	m_colorBuffer.reserve(m_setup.viewport.width * m_setup.viewport.height);
}

Rasterizer::~Rasterizer()
{
	m_colorBuffer.clear();
	m_zBuffer.clear();
}

void Rasterizer::Draw(const vector<vec3>& vertices, const mat4& view, const mat4& proj)
{
}