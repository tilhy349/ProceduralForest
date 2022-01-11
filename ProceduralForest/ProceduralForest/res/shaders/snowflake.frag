#version 330 core

in vec2 texCoord;

out vec4 color;
uniform sampler2D u_Texture;

void main(){

	vec4 texColor = texture(u_Texture, texCoord);
	color = texColor - vec4(0.0, 0.0, 0.0, 0.3); //Make the snowflake a bit more transparent
};