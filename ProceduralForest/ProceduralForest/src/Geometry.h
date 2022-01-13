#pragma once
#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include <memory>
#include <vector>

//Geometry class expects each vertex to be size: 3 * 3 * 2 * sizeof(float)
class Geometry {

private:

	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<IndexBuffer> m_IndexBuffer;
	std::unique_ptr<VertexBuffer> m_VB;

	std::vector<float> vertices;
	std::vector<unsigned int> indices;

public:

	Geometry(std::vector<float>* verts, std::vector<unsigned int>* inds);
	~Geometry();

	void Bind() const;
	inline unsigned int GetCount() const { return m_IndexBuffer->GetCount(); };

	/// <summary>
	/// Render the geometry using the specified shader. The draw call is executed using DrawElements.
	/// </summary>
	/// <param name="shader">The shader to render the geometry.</param>
	void Render(Shader& shader);
};
