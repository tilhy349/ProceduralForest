#pragma once
#include <GL/glew.h>
#include <iostream>

//Simple error handling

#define ASSERT(x) if (!(x)) __debugbreak(); //Only works in vs
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall())


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
