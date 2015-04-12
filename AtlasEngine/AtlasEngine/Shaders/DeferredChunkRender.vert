#version 430 core

layout (location = 0) in uint x6_y6_z6_n3_null3_b8;

out VS_OUT 
{
	vec3 Normal;
	vec3 Color;
	flat uint MaterialID;
} vs_out;

layout(std140, binding = 2) uniform TransformBlock
{
// Member					Base Align		Aligned Offset		End
	mat4 Model;				//		16					0			64
	mat4 View;				//		16					64			128
	mat4 Projection;		//		16					128			192
	mat4 InvProjection;     //      16                  192         256
} Transforms;

layout(std430, binding = 3) buffer BlockInfoBlock
{
	vec4 BlockColors[];
};

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
	vs_out.Color = BlockColors[x6_y6_z6_n3_null3_b8 & 0xFF].xyz;

	// Unpack normal and lookup with table
	vec3 WorldNormal = BlockNormals[(x6_y6_z6_n3_null3_b8 & 0x3800) >> 11];
	vs_out.Normal = mat3(Transforms.View) * mat3(Transforms.Model) * WorldNormal;

	// Unpack world position
	vec4 Position;
	Position.x = float((x6_y6_z6_n3_null3_b8 & 0xFC000000) >> 26);
	Position.y = float((x6_y6_z6_n3_null3_b8 & 0x03F00000) >> 20);
	Position.z = float((x6_y6_z6_n3_null3_b8 & 0x000FC000) >> 14);
	Position.w = 1.0;

	vs_out.MaterialID = uint(gl_VertexID);

	gl_Position = Transforms.Projection * Transforms.View * Transforms.Model * Position;
}