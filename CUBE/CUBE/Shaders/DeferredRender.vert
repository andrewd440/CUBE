#version 430 core

#include "UniformBlocks.glsl"

layout ( location = 0 ) in vec3 vPosition;
layout( location = 1 ) in vec3 vNormal;
layout( location = 2 ) in vec4 vColor;

out VS_OUT 
{
	vec3 Normal;
	vec3 Color;
	flat uint MaterialID;
} vs_out;

void main()
{
	vs_out.Color = vColor.xyz;
	vs_out.Normal = mat3(Transforms.View) * mat3(Transforms.Model) * vNormal;
	vs_out.MaterialID = uint(gl_VertexID);

	gl_Position = Transforms.Projection * Transforms.View * Transforms.Model * vec4(vPosition, 1.0);
}