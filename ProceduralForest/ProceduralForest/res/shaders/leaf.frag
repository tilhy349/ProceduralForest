#version 330 core

in vec2 texCoord;
//in vec3 colorOffset;

out vec4 color;

void main(){

	color = vec4(texCoord.x, texCoord.y, 0.0f, 1.0);
	//if(colorOffset.x < 5)
		//color = vec4(1.0
};