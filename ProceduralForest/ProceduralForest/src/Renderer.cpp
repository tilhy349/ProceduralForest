#include "Renderer.h"

#include <iostream>

void GLClearError() {

    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall() {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << ")\n";
        return false;
    }
    return true;
}

void Renderer::Clear() const
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.65f, 0.85f, 1.0f, 1.0f);
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    shader.Bind();
    va.Bind();
    ib.Bind();

    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

