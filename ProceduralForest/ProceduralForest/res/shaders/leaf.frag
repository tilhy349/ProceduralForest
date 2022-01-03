#version 330 core

in vec2 texCoord;

out vec4 color;
uniform sampler2D u_Texture;

uniform float time; //Time variable
uniform int season;

void main(){

	vec4 texColor = texture(u_Texture, texCoord);
	float blendVal = 0.5 * sin(time * 3.14 / 10) + 0.5;
	//float degree = mod(time * 3.14/ 10, 3.14 * 2);
	//if( degree > 3.14 / 2 && degree < 3.14 * 3 / 2)
		//blendVal = 0.0; 
	//TODO: FIND THE VALUES FOR WHICH TIME IS BETWEEN PI/2 and 3PI/2
	color = texColor + blendVal * vec4(1.0f, 0.0f, 0.0f, 0.0f);
};