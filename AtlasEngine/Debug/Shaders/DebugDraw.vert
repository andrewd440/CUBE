#version 430 core

layout ( location = 0 ) in vec4 vPosition;
layout( location = 2 ) in vec4 vColor;

layout(std140, binding = 2) uniform TransformBlock
{
// Member					Base Align		Aligned Offset		End
	mat4 Model;				//		16					0			64
	mat4 View;				//		16					64			128
	mat4 Projection;		//		16					128			192
} Transforms;

out vec4 fColor;

void main()
{
    fColor = vColor;
    gl_Position = Transforms.Projection * Transforms.View * vPosition;
}