#version 430

layout (location = 0) in vec3 Position;

uniform mat4 gWVP;

void main()
{
	gl_Position = gWVP * vec4(Position.xyz, 1.0);
}