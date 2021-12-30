#include "Geometry.h"

Geometry::Geometry(std::vector<float>* verts, std::vector<unsigned int>* inds)
{
    vertices = *verts;
    indices = *inds;

    m_VAO = new VertexArray();

    VertexBuffer vb(static_cast<void*>(vertices.data()), vertices.size() * sizeof(float));

    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(3);
    layout.Push<float>(2);

    m_VAO->AddBuffer(vb, layout);

    m_IndexBuffer = new IndexBuffer(static_cast<unsigned int*>(indices.data()), indices.size());

    //m_VAO->Bind();
    //m_IndexBuffer->Bind();
    //Bind();

    //glDrawElements(GL_TRIANGLES, m_IndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);
}

Geometry::~Geometry()
{
}

void Geometry::Bind() const
{
    m_VAO->Bind();
    m_IndexBuffer->Bind();
}

void Geometry::Render(Shader& shader)
{
    shader.Bind();
    Bind();

    glDrawElements(GL_TRIANGLES, GetCount(), GL_UNSIGNED_INT, nullptr);
}


