#version 330 core

in vec2 texCoord;

out vec4 color;
uniform sampler2D u_Texture;

void main(){

	vec4 texColor = texture(u_Texture, texCoord);
	//TODO: FIND THE VALUES FOR WHICH TIME IS BETWEEN PI/2 and 3PI/2
	color = texColor;
};