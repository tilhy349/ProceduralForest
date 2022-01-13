#version 330 core

layout(location = 0) in  vec3 inPosition;
layout(location = 1) in  vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
out vec3 exNormal; // Phong
out vec3 exSurface; // Phong (specular)
out vec2 exTexCoord;
out float vertexHeight;
out vec2 pos;

uniform mat4 modelviewMatrix;
uniform mat4 projectionMatrix;

void main(void)
{
	exNormal = inverse(transpose(mat3(modelviewMatrix))) * inNormal; // Phong, normal transformation

	exSurface = vec3(modelviewMatrix * vec4(inPosition, 1.0)); // Don't include projection here - we only want to go to view coordinates

	exTexCoord = inTexCoord;

	vertexHeight = inPosition.y;

	gl_Position = projectionMatrix * modelviewMatrix * vec4(inPosition, 1.0); // This should include projection

	pos = vec2(inPosition.x, inPosition.z);
}
