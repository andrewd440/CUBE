#version 430 core

//#pragma optimize(off)
//#pragma debug(off)

//uniform float TimeDelta;

uniform sampler2D Texture;

void main()
{
	float a = texture2D(Texture, gl_TexCoord[0].xy).r;
	gl_FragColor = vec4(gl_Color.rgb, gl_Color.a*a);

	//gl_FragColor = vec4(1,1,1,1);
}