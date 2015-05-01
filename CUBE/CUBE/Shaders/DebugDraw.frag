#version 430 core

//#pragma optimize(off)
//#pragma debug(off)

in vec4 fColor;

void main()
{
	gl_FragColor = fColor;
}