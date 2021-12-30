#pragma once
#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include <memory>
#include <vector>

class Geometry {

public:
	VertexArray* m_VAO;
	IndexBuffer* m_IndexBuffer;

	std::vector<float> vertices;
	std::vector<unsigned int> indices;

public:

	Geometry(std::vector<float>* verts, std::vector<unsigned int>* inds);
	~Geometry();

	void Bind() const;
	inline unsigned int GetCount() const { return m_IndexBuffer->GetCount(); };

	void Render(Shader& shader);
};
