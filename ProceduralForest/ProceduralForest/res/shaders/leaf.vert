#version 330 core

layout(location = 0) in vec3 position; 
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 offsetMatrixCol1; 
layout(location = 3) in vec3 offsetMatrixCol2;
layout(location = 4) in vec3 offsetMatrixCol3;
layout(location = 5) in vec3 offsetMatrixCol4;

out vec2 texCoord;
out vec3 colorOffset;

uniform mat4 scalingMat;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(){

    mat4 modelOffset = mat4(vec4(offsetMatrixCol1, 0.0), vec4(offsetMatrixCol2, 0.0), vec4(offsetMatrixCol3, 0.0), vec4(offsetMatrixCol4, 1.0));

    gl_Position =  projectionMatrix * viewMatrix * modelMatrix * modelOffset * scalingMat * vec4(position, 1.0); 
    texCoord = inTexCoord;
    //colorOffset = offset;
}; 