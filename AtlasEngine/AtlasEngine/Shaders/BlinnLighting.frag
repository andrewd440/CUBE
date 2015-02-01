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

uniform	vec3 SpecularProduct = vec3(.0, .0, .0);
uniform	vec3 DiffuseProduct = vec3( .5, .5, .5);
uniform	vec3 AmbientProduct = vec3(.2, .2, .2);
uniform	float Shininess = 64.0;

uniform vec4 FogColor = vec4(.1, .2, .3, 1);
uniform float FogDensity = 0.0003;

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


	float z = gl_FragCoord.z / gl_FragCoord.w;
	float fog = clamp(exp(-FogDensity * z * z), 0.05, 1);
 
	gl_FragColor = mix(FogColor, vec4(Diffuse + Spec + AmbientProduct, 1.0), fog);

	//gl_FragColor = fs_in.Color;
	//gl_FragColor = vec4(Diffuse + Spec + AmbientProduct, 1.0);
}