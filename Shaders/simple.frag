#version 330

uniform mat4x4 mvMat;
uniform mat4x4 projMat;
uniform ivec4 name;

in vec3 fposition;
in vec3 fnormal;

out vec4 fragColour;
out ivec4 outName;

void main() {
	//fragColour = vec4(0,0,0,1);
	//return;
	vec3 normal = fnormal;
	vec3 lightDir = vec3(0,0,1);
	float ambient = 0.3;
	//float diffusion = 0.6*clamp(dot(normal,lightDir),0,1);
	float diffusion = 0.6*abs(dot(normal,lightDir));
	vec3 eyeDir = normalize(-fposition);
	vec3 hv = normalize(eyeDir+lightDir);
	//float specular = 0.5*pow(clamp(dot(hv,normal),0,1),16);
	float specular = 0.5*pow(abs(dot(hv,normal)),16);
	vec4 thisColor = vec4(1,0,0,1);
	fragColour = thisColor*(ambient+diffusion);
	fragColour += vec4(specular,specular,specular,0);
	fragColour.a = 1;
	//fragColour.a = pow((1-abs(dot(normal,eyeDir))),2);
	outName = name;
}