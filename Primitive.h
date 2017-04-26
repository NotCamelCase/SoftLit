#pragma once

namespace softlit
{
	typedef void* UniformBuffer;

	// vertex_shader to be run per-vertex. returned vec4() is vertex converted to clip-coordinates
	typedef glm::vec4(*vertex_shader)(const glm::vec3& pos, UniformBuffer ubo);

	// fragment_shader to output final render target values
	typedef glm::vec4(*fragment_shader)();

	typedef std::vector<glm::vec3> VertexBuffer;
	typedef std::vector<uint64_t> IndexBuffer;

	class Primitive
	{
	public:
		Primitive(const PrimitiveTopology top);
		~Primitive();

		const VertexBuffer& getVertexBuffer() const { return m_vertexBuffer; }
		const IndexBuffer& getIndexBuffer() const { return m_indexBuffer; }

		void setVertexBuffer(const VertexBuffer& vb) { m_vertexBuffer = vb; }
		void setIndexBuffer(const IndexBuffer& ib) { m_indexBuffer = ib; }

		const PrimitiveTopology getPrimitiveTopology() const { return m_topology; }

		vertex_shader VS() { return m_VS; }
		fragment_shader FS() { return m_FS; }

		void VS(const vertex_shader vs) { m_VS = vs; }
		void FS(const fragment_shader fs) { m_FS = fs; }

		UniformBuffer UBO() { return m_ubo; }
		void UBO(UniformBuffer ubo) { m_ubo = ubo; }

	private:
		VertexBuffer m_vertexBuffer;
		IndexBuffer m_indexBuffer;

		PrimitiveTopology m_topology = PrimitiveTopology::TRIANGLE_LIST;

		UniformBuffer m_ubo = nullptr;
		vertex_shader m_VS = nullptr;
		fragment_shader m_FS = nullptr;
	};
}