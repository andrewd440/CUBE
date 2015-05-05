
layout(std140, binding = 2) uniform TransformBlock
{
// Member					Base Align		Aligned Offset		End
	mat4 Model;				//		16					0			64
	mat4 View;				//		16					64			128
	mat4 Projection;		//		16					128			192
	mat4 InvProjection;		//      16                  192         256
} Transforms;

layout(std140, binding = 4) uniform ResolutionBlock
{
	uvec2 Resolution;
};

layout(std140, binding = 1) uniform ProjectionInfoBlock
{
	float Near;
	float Far;
} ProjectionInfo;
