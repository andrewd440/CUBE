#version 430 core

//#pragma optimize(off)
//#pragma debug(off)

layout ( location = 0 ) in vec4 vPosition;
layout( location = 2 ) in vec4 vColor;

layout(std140, binding = 0) uniform TransformBlock
{
// Member					Base Align		Aligned Offset		End
	mat4 View;			//		16					0			64
	mat4 Projection;	//		16					64			128
	vec4 Translation;  //		16					128			
} Transforms;

out vec4 fColor;

void main()
{
    fColor = vColor;
    gl_Position = Transforms.Projection * Transforms.View * vPosition;
}