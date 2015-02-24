#version 430 core

//#pragma optimize(off)
//#pragma debug(off)

layout ( location = 0 ) in vec4 vPosition;
layout( location = 2 ) in vec4 vColor;
layout( location = 3 ) in vec2 vUV;

uniform mat4 Projection;

void main()
{
    gl_TexCoord[0].xy = vUV.xy;
    gl_FrontColor     = vColor;
    gl_Position       = Projection * vPosition;
}