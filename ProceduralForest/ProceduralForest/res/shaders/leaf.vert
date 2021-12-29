#version 330 core

layout(location = 0) in vec3 position; 
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec4 offsetMatrixRow1; 
layout(location = 3) in vec4 offsetMatrixRow2;
layout(location = 4) in vec4 offsetMatrixRow3;
layout(location = 5) in vec4 offsetMatrixRow4;

out vec2 texCoord;
out vec3 colorOffset;

uniform mat4 modelviewMatrix;
uniform mat4 projectionMatrix;

void main(){
    mat4 modelOffset = mat4(offsetMatrixRow1, offsetMatrixRow2, offsetMatrixRow3, offsetMatrixRow4);
    gl_Position =  projectionMatrix * modelviewMatrix * modelOffset * vec4(position, 1.0); //TODO fix logic
    texCoord = inTexCoord;
    //colorOffset = offset;
}; 