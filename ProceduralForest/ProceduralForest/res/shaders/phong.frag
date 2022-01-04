#version 150

// Simplified Phong: No materials, only one, hard coded light source
// and no ambient

out vec4 outColor;
in vec3 exNormal; // Phong
in vec3 exSurface; // Phong (specular)
in vec2 exTexCoord;
in float vertexHeight;

vec2 random2(vec2 st)
{
    st = vec2( dot(st,vec2(127.1,311.7)),
              dot(st,vec2(269.5,183.3)) );
    return -1.0 + 2.0*fract(sin(st)*43758.5453123);
}

// Gradient Noise by Inigo Quilez - iq/2013
// https://www.shadertoy.com/view/XdXGW8
float noise(vec2 st)
{
    vec2 i = floor(st);
    vec2 f = fract(st);

    vec2 u = f*f*(3.0-2.0*f);

    return mix( mix( dot( random2(i + vec2(0.0,0.0) ), f - vec2(0.0,0.0) ),
                     dot( random2(i + vec2(1.0,0.0) ), f - vec2(1.0,0.0) ), u.x),
                mix( dot( random2(i + vec2(0.0,1.0) ), f - vec2(0.0,1.0) ),
                     dot( random2(i + vec2(1.0,1.0) ), f - vec2(1.0,1.0) ), u.x), u.y);
}

void main(void)
{
	const vec3 light = vec3(0.58, 0.58, 0.58); // Given in VIEW coordinates! You will usually give light in world coordinates.
	float diffuse, specular, shade;

	// Diffuse
	diffuse = dot(normalize(exNormal), light);
	diffuse = max(0.0, diffuse); // No negative light

	// Specular
	vec3 r = reflect(-light, normalize(exNormal));
	vec3 v = normalize(-exSurface); // View direction
	specular = dot(r, v);
	if (specular > 0.0)
		specular = 1.0 * pow(specular, 150.0);
	specular = max(specular, 0.0);
	shade = 0.7*diffuse + 1.0*specular + 0.3;

	//vec4 color = vec4(exTexCoord.x, exTexCoord.y, exTexCoord.y, 1.0);
	vec4 color = vec4(vertexHeight*5, vertexHeight*2 + 0.2, 0.5, 1);
	//color = vec4(0.5, 0.5, 0.0, 1.0);

	outColor = vec4(shade, shade, shade, 1.0) * color + noise(vec2(exTexCoord * 10)) * vec4(0.5, 0.5, 0.0, 1.0);
	outColor = vec4(shade, shade, shade, 1.0) * color;
}
