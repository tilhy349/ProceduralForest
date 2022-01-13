#include "Geometry.h"

Geometry::Geometry(std::vector<float>* verts, std::vector<unsigned int>* inds)
{
    //Store the data from the vertices and indices in the geomerty instance
    vertices = *verts;
    indices = *inds;

    m_VAO = std::make_unique<VertexArray>();

    m_VB = std::make_unique<VertexBuffer>(static_cast<void*>(vertices.data()), vertices.size() * sizeof(float));

    //Specify the layout of the buffer, 3 floats + 3 floats + 2 floats
    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(3);
    layout.Push<float>(2);

    m_VAO->AddBuffer(*m_VB, layout);

    m_IndexBuffer = std::make_unique <IndexBuffer>(static_cast<unsigned int*>(indices.data()), indices.size());
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
    //Bind the needed buffers
    shader.Bind();
    Bind();

    glDrawElements(GL_TRIANGLES, GetCount(), GL_UNSIGNED_INT, nullptr);
}


