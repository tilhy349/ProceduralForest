#pragma once

#include <GL/glew.h>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Geometry.h"
#include "Shader.h"

//Simple error handling

#define ASSERT(x) if (!(x)) __debugbreak(); //Only works in vs
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall())


void GLClearError();
bool GLLogCall();

class Renderer {

public:
    void Clear() const;

    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
    void DrawModel(const Geometry& model, const Shader& shader) const;
};
