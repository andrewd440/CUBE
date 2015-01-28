#version 430 core

//#pragma optimize(off)
//#pragma debug(off)

//uniform float TimeDelta;

in VS_OUT 
{
	vec4 Color;
	vec3 Normal;
	vec3 Light;
	vec3 Viewer;
} fs_in;

uniform	vec3 SpecularProduct = vec3(.1, .1, .1);
uniform	vec3 DiffuseProduct = vec3( .5, .5, .5);
uniform	vec3 AmbientProduct = vec3(.1, .1, .1);
uniform	float Shininess = 64.0;

void main()
{
	vec3 N = normalize(fs_in.Normal);
	vec3 L = normalize(fs_in.Light);
	vec3 V = normalize(fs_in.Viewer);
	
	vec3 H = normalize(L + V);

	vec3 Diffuse = max(0.0, dot(N, L)) * fs_in.Color.xyz;
	vec3 Spec = pow(max( 0.0, dot(H, N)), Shininess) * SpecularProduct;
	if(dot(N, L) < 0.0)
		Spec = vec3(0,0,0);

	//gl_FragColor = fs_in.Color;
	gl_FragColor = vec4(Diffuse + Spec + AmbientProduct, 1.0);
}