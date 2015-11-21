#version 330

uniform mat4x4 mvMat;
uniform mat4x4 projMat;

in vec3 position;
in vec4 color;
in ivec4 name;

out vec4 fcolor;
flat out ivec4 fname;

void main() {
	fcolor = color;
	fname = name;
	vec4 modelPos = mvMat*vec4(position,1);
	gl_Position = projMat*modelPos;
}