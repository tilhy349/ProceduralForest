#include "Geometry.h"

Geometry::Geometry(std::vector<float>* vertices, std::vector<unsigned int>* indices)
{
    m_VAO = std::make_unique<VertexArray>();

    VertexBuffer vb(static_cast<void*>(vertices->data()), vertices->size() * sizeof(float));

    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(3);
    layout.Push<float>(2);

    m_VAO->AddBuffer(vb, layout);

    m_IndexBuffer = std::make_unique<IndexBuffer>(static_cast<unsigned int*>(indices->data()), indices->size());
}

Geometry::~Geometry()
{
}

void Geometry::Bind() const
{
    m_VAO->Bind();
    m_IndexBuffer->Bind();
}


