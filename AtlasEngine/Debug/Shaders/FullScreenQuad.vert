#version 430 core

// Shader for rendering a full screen quad. Use glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

layout(std140, binding = 2) uniform TransformBlock
{
// Member					Base Align		Aligned Offset		End
	mat4 Model;				//		16					0			64
	mat4 View;				//		16					64			128
	mat4 Projection;		//		16					128			192
	mat4 InvViewProjection; //      16                  192         256
} Transforms;

out vec3 FrustumCorner;

void main()
{
	const vec4 QuadVerts[4] = vec4[4](	vec4(-1.0, -1.0, 1.0, 1.0),
										vec4( 1.0, -1.0, 1.0, 1.0),
										vec4(-1.0,  1.0, 1.0, 1.0),
										vec4( 1.0,  1.0, 1.0, 1.0));

	FrustumCorner = (inverse(Transforms.Projection) * QuadVerts[gl_VertexID]).xyz;
	FrustumCorner = vec3(FrustumCorner.xy / FrustumCorner.z, 1.0);
	gl_Position = QuadVerts[gl_VertexID];
}