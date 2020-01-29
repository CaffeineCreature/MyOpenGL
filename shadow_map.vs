#version 460

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;

uniform mat4 gWVP;
uniform mat4 gWorld;

out vec3 WorldPos;
                  
void main()
{
    vec4 Pos4 = vec4(Position, 1.0);
    gl_Position = gWVP * Pos4;
    gl_Position.y = -gl_Position.y;
    WorldPos = (gWorld * Pos4).xyz;    
}
