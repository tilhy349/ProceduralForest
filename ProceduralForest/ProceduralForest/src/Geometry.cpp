#include "Geometry.h"

Geometry::Geometry() 
{
    glGenVertexArrays(1, &m_RendererID);
    glBindVertexArray(m_RendererID);

    m_VAO = 0;
    m_IndexBuffer = 0;
}

void Geometry::CreateCylinder()
{
    float vertices[] = {
       -50.0f, -50.0f, 0.0f, 0.0f,
        50.0f, -50.0f, 1.0f, 0.0f,
        50.0f,  50.0f, 1.0f, 1.0f,
       -50.0f,  50.0f, 0.0f, 1.0f
    };

    unsigned int indices[] = {
       0, 1, 2,
       2, 3, 0
    };

    m_VAO = std::make_unique<VertexArray>();

    VertexBuffer vb(vertices, 4 * 4 * sizeof(float));

    VertexBufferLayout layout;
    layout.Push<float>(2);
    layout.Push<float>(2);

    m_VAO->AddBuffer(vb, layout);

    m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);
}
