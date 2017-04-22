#pragma once

namespace softlit
{
	class Primitive
	{
	public:
		Primitive(const PrimitiveTopology top);
		~Primitive();

		const std::vector<glm::vec3>& getVertexBuffer() const { return m_vertexBuffer; }
		const std::vector<uint64_t>& getIndexBuffer() const { return m_indexBuffer; }

		void setVertexBuffer(const std::vector<glm::vec3>& vb) { m_vertexBuffer = vb; }
		void setIndexBuffer(const std::vector<uint64_t>& ib) { m_indexBuffer = ib; }

		const PrimitiveTopology getPrimitiveTopology() const { return m_topology; }

	private:
		std::vector<glm::vec3> m_vertexBuffer;
		std::vector<uint64_t> m_indexBuffer;

		PrimitiveTopology m_topology;
	};
}