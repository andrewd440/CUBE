#version 430 core

//#pragma optimize(off)
//#pragma debug(off)

#include "UniformBlocks.glsl"

layout ( location = 0 ) in vec4 vPosition;
layout( location = 2 ) in vec4 vColor;

out vec4 fColor;

void main()
{
    fColor = vColor;
    gl_Position = Transforms.Projection * Transforms.View * vPosition;
}