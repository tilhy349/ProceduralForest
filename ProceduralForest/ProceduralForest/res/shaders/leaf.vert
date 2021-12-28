#version 330 core

layout(location = 0) in vec3 position; 
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 offset; 

out vec2 texCoord;
out vec3 colorOffset;

uniform mat4 modelviewMatrix;
uniform mat4 projectionMatrix;

void main(){
    gl_Position =  projectionMatrix * modelviewMatrix * vec4(position + offset, 1.0);
    texCoord = inTexCoord;
    colorOffset = offset;
}; 