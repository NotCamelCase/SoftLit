#pragma once

namespace softlit
{
	typedef std::vector<glm::vec3> VertexBuffer;
	typedef std::vector<uint64_t> IndexBuffer;

	class Primitive
	{
	public:
		Primitive();
		~Primitive();

		const VertexBuffer& getVertexBuffer() const { return m_vertexBuffer; }
		const IndexBuffer& getIndexBuffer() const { return m_indexBuffer; }

		void setVertexBuffer(const VertexBuffer& vb) { m_vertexBuffer = vb; }
		void setIndexBuffer(const IndexBuffer& ib) { m_indexBuffer = ib; }

	private:
		VertexBuffer m_vertexBuffer;
		IndexBuffer m_indexBuffer;
	};
}