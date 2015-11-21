#version 330

uniform mat4x4 mvMat;
uniform mat4x4 projMat;
uniform vec3 color;
uniform int bUseTextureColor;
uniform sampler3D colorTexture;
uniform vec3 volSize;

in vec3 position;
in vec3 normal;
in ivec4 name;

out vec3 fposition;
out vec3 fnormal;
out vec3 rawPos;
flat out ivec4 fname;

void main (void)
{
	rawPos = position;
	fnormal = (transpose(inverse(mvMat))*vec4(normal,0)).xyz;
	fposition = vec3(mvMat*vec4(position,1));
	fname = name;
	gl_Position = projMat*mvMat*vec4(position,1);
}