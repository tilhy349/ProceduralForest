#version 150

out vec4 outColor;

in vec2 texCoord;
uniform sampler2D tex;
in vec3 exNormal;

void main(void)
{
	vec4 t = texture(tex, texCoord);
	vec3 n = normalize(exNormal);

	float shade = n.z;
		outColor = t * (shade+0.5);
	outColor.a = 1.0;
	
}
