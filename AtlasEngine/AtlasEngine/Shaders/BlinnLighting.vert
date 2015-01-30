#version 430 core

//#pragma optimize(off)
//#pragma debug(off)

layout ( location = 0 ) in vec4 vPosition;
layout( location = 1 ) in vec3 vNormal;
layout( location = 2 ) in vec4 vColor;

out VS_OUT 
{
	vec4 Color;
	vec3 Normal;
	vec3 Light;
	vec3 Viewer;
} vs_out;

//uniform float TimeDelta;

layout(std140, binding = 0) uniform TransformBlock
{
// Member					Base Align		Aligned Offset		End
	mat4 View;			//		16					0			64
	mat4 Projection;	//		16					64			128
	vec4 Translation;  //		16					128			
} Transforms;


uniform	vec4 LightPosition = vec4(20, 100000, 100000, 1);

void main()
{
	vec4 ViewPosition = Transforms.View * ( Transforms.Translation + vPosition);

	vs_out.Normal = mat3(Transforms.View) * vNormal;
	vs_out.Light = (Transforms.View * LightPosition).xyz - ViewPosition.xyz;
	vs_out.Viewer = -ViewPosition.xyz;

	gl_Position = Transforms.Projection * ViewPosition;
	vs_out.Color = vColor;
}