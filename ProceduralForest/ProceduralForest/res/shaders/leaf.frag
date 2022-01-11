#version 330 core

in vec2 texCoord;

out vec4 color;
uniform sampler2D u_Texture;

uniform float time; //Time variable used to compute blend value

void main(){

	vec4 texColor = texture(u_Texture, texCoord);
	float blendVal = 0.5 * sin(time * 3.14 / 13 + 3 * 3.14 / 2) + 0.5;
	color = texColor + blendVal * vec4(1.0f, 0.0f, 0.0f, 0.0f);
};