#version 440

layout (location = 0) out vec4 FragColor;

uniform sampler2D gFrameBuffer;

uniform vec2 gScreenSize;

vec3 ACESFilm( vec3 x )
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / gScreenSize;
}

void main()
{
	vec2 texCoord = CalcTexCoord();

	FragColor = vec4(ACESFilm(texture(gFrameBuffer, texCoord).xyz), 1.0);
}