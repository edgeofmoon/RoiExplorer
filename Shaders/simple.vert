#version 330

uniform mat4x4 mvMat;
uniform mat4x4 projMat;
uniform ivec4 name;

in vec3 position;
in vec3 normal;

out vec3 fposition;
out vec3 fnormal;

void main() {
	fnormal = normalize((inverse(mvMat)*vec4(normal,0)).xyz);
	vec4 mvPos = mvMat*vec4(position,1);
	fposition = mvPos.xyz;
	gl_Position = projMat*mvPos;
}