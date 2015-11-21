#version 330

uniform mat4x4 mvMat;
uniform mat4x4 projMat;

in vec4 fcolor;
flat in ivec4 fname;

out vec4 fragColour;
out ivec4 name;

void main() {
	name = fname;
	fragColour = fcolor;
}