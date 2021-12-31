#pragma once
#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include <memory>
#include <vector>

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

	void Render(Shader& shader);
};
