#pragma once
#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include <memory>
#include <vector>

class Geometry {

public:
	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<IndexBuffer> m_IndexBuffer;

public:

	Geometry(std::vector<float>* vertices, std::vector<unsigned int>* indices);
	~Geometry();

	void Bind() const;
	inline unsigned int GetCount() const { return m_IndexBuffer->GetCount(); };
};
