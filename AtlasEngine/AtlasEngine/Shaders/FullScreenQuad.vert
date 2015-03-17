#version 430 core

// Shader for rendering a full screen quad. Use glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

void main()
{
	const vec4 QuadVerts[4] = vec4[4](	vec4(-1.0, -1.0, 1.0, 1.0),
										vec4( 1.0, -1.0, 1.0, 1.0),
										vec4(-1.0,  1.0, 1.0, 1.0),
										vec4( 1.0,  1.0, 1.0, 1.0));

	gl_Position = QuadVerts[gl_VertexID];
}