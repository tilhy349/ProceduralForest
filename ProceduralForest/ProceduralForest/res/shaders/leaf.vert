#version 330 core

layout(location = 0) in vec3 position; 
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 offsetMatrixCol1; 
layout(location = 3) in vec3 offsetMatrixCol2;
layout(location = 4) in vec3 offsetMatrixCol3;
layout(location = 5) in vec3 offsetMatrixCol4;

out vec2 texCoord;

uniform float updatedYPos; //Updated position used for fall season

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

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

void main(){
    //Construct mat4 modelOffset matrix using the 4 input buffer values
    mat4 modelOffset = mat4(vec4(offsetMatrixCol1, 0.0), vec4(offsetMatrixCol2, 0.0), vec4(offsetMatrixCol3, 0.0), vec4(offsetMatrixCol4, 1.0));
    
    //Calculate the position of the instance to be used to create gradient noise
    vec4 posWOffset = (modelOffset * vec4(1.0));
    
    //Caluculate gradient noise
    float noiseValue = 3.0 * abs(noise(vec2(posWOffset.x * 10, posWOffset.z * 10)));
    
    //Create translation matrix using updatedYPos and noiseValue
    mat4 translation = mat4(1.0, 0.0, 0.0, 0.0, 
                            0.0, 1.0, 0.0, 0.0, 
                            0.0, 0.0, 1.0, 0.0,
                            0.0, updatedYPos * noiseValue, 0.0, 1.0);
    gl_Position =  projectionMatrix * viewMatrix * translation * modelOffset * modelMatrix * vec4(position, 1.0); 
    texCoord = inTexCoord;
}; 