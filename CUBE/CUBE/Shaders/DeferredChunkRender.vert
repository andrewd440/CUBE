#version 430 core

#include "UniformBlocks.glsl"

layout (location = 0) in vec4 iPosition;
layout (location = 4) in uint Normal8_Block8;

out VS_OUT 
{
	vec3 Normal;
	vec3 Color;
	flat uint MaterialID;
} vs_out;

layout(binding = 4) uniform sampler1D BlockColors;

const vec3 BlockNormals[6] =
{
	vec3( 1,  0,  0),
	vec3(-1,  0,  0),
	vec3( 0,  1,  0),
	vec3( 0, -1,  0),
	vec3( 0,  0,  1),
	vec3( 0,  0, -1)
};

void main()
{
	// Unpack color
	vs_out.Color = texelFetch(BlockColors, int(Normal8_Block8 & 0xFF), 0).xyz;

	// Unpack normal and lookup with table
	vec3 WorldNormal = BlockNormals[(Normal8_Block8 & 0xFF00) >> 8];
	vs_out.Normal = mat3(Transforms.View) * WorldNormal;

	vs_out.MaterialID = uint(gl_VertexID);

	gl_Position = Transforms.Projection * Transforms.View * iPosition;
}