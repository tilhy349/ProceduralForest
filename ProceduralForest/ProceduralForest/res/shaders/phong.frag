#version 150

// Simplified Phong: No materials, only one, hard coded light source
// and no ambient

out vec4 outColor;
in vec3 exNormal; // Phong
in vec3 exSurface; // Phong (specular)
in vec2 exTexCoord;
in float vertexHeight;
in vec2 pos; //Position not multiplied with view

uniform float winter;

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

// Voronoise Created by inigo quilez - iq/2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
// http://iquilezles.org/www/articles/voronoise/voronoise.htm
vec3 hash3( vec2 p ) {
    vec3 q = vec3( dot(p,vec2(127.1,311.7)),
                   dot(p,vec2(269.5,183.3)),
                   dot(p,vec2(419.2,371.9)) );
    return fract(sin(q)*43758.5453);
}

float iqnoise( in vec2 x, float u, float v )
{
    vec2 p = floor(x);
    vec2 f = fract(x);

    float k = 1.0+63.0*pow(1.0-v,4.0);

    float va = 0.0;
    float wt = 0.0;
    for (int j=-2; j<=2; j++)
	{
        for (int i=-2; i<=2; i++)
		{
            vec2 g = vec2(float(i),float(j));
            vec3 o = hash3(p + g)*vec3(u,u,1.0);
            vec2 r = g - f + o.xy;
            float d = dot(r,r);
            float ww = pow( 1.0-smoothstep(0.0,1.414,sqrt(d)), k );
            va += o.z*ww;
            wt += ww;
        }
    }

    return va/wt;
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
	shade = 0.7*diffuse + 0.05*specular + 0.3;

	//vec4 color = vec4(exTexCoord.x, exTexCoord.y, exTexCoord.y, 1.0);
	//vec4 color = vec4(vertexHeight*5, vertexHeight*2 + 0.2, 0.5, 1) + vec4(0.9) * winter;
    float noiseVal = 0.3 * abs(iqnoise(vec2(100 * pos.x, 100 * pos.y), 1.0, 0.1)) * (1 - winter);
    float snowNoiseVal = 10 * abs(noise(vec2(0.1 * pos.x, 0.1 * pos.y)));
	vec4 color = vec4(vec3(min(0.40 + noiseVal, 0.49), min(0.25 + noiseVal, 0.32), min(0.10 + noiseVal, 0.15)) , 1.0) + vec4(0.8) * winter;
    //vec4 color = vec4(noiseVal, noiseVal, noiseVal, 1.0) + vec4(0.9)* winter;
    //vec4 color = vec4(0.5, 0.6, 0.2, 1.0) + vec4(vec3(0.3, 0.3, 0.1) + noiseVal, 1.0);
    color = color + vec4(-0.1, -0.05, 0.0, 0.0) * (1 - vertexHeight);

	//outColor = vec4(shade, shade, shade, 1.0) * color + noise(vec2(exTexCoord * 10)) * vec4(0.5, 0.5, 0.0, 1.0);
	outColor = vec4(shade, shade, shade, 1.0) * color - vec4(0.1, 0.1, 0.1, 0.0) * winter;
    //outColor = color;
}
