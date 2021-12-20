#pragma once
#include "Renderer.h"
#include "VertexBufferLayout.h"
#include <memory>

class Geometry {

public:
	unsigned int m_RendererID;
	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<IndexBuffer> m_IndexBuffer;

public:

	Geometry();
	~Geometry();

	void CreateCylinder();

	//void Render(Shader& shader);
};
