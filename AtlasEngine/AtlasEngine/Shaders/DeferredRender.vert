#version 430 core

layout ( location = 0 ) in vec4 vPosition; // AO factor is in w component
layout( location = 1 ) in vec3 vNormal;
layout( location = 2 ) in vec4 vColor;

out VS_OUT 
{
	vec3 WorldCoord;
	vec3 Normal;
	vec3 Color;
	float AmbientOcclusion;
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


void main()
{
	mat4 ModelView = Transforms.View * Transforms.Model;
	vs_out.WorldCoord = (Transforms.Model * vec4(vPosition.xyz, 1.0)).xyz;
	vs_out.Color = vec3(vColor);
	vs_out.Normal = mat3(Transforms.Model) * vNormal;
	vs_out.AmbientOcclusion = vPosition.w;
	vs_out.MaterialID = uint(gl_VertexID);

	gl_Position = Transforms.Projection * ModelView * vec4(vPosition.xyz, 1.0);
}