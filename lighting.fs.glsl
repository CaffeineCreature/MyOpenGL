#version 430 core                                                                           
                                                                                            
const int MAX_POINT_LIGHTS = 2;                                                             
const int MAX_SPOT_LIGHTS = 2;

in vec2 TexCoord_FS_in;                                                                     
in vec3 Normal_FS_in;                                                                       
in vec3 WorldPos_FS_in;                                                                     
                                                                                            
out vec4 FragColor;                                                                         
                                                                                            
struct BaseLight                                                                            
{                                                                                           
    vec3 Color;                                                                             
    float AmbientIntensity;                                                                 
    float DiffuseIntensity;                                                                 
};                                                                                          
                                                                                            
struct DirectionalLight                                                                     
{                                                                                           
    BaseLight Base;                                                                  
    vec3 Direction;                                                                         
};                                                                                          
                                                                                            
struct Attenuation                                                                          
{                                                                                           
    float Constant;                                                                         
    float Linear;                                                                           
    float Exp;                                                                              
};                                                                                          
                                                                                            
struct PointLight                                                                           
{                                                                                           
    BaseLight Base;                                                                  
    vec3 Position;                                                                          
    Attenuation Atten;                                                                      
};                                                                                          
                                                                                            
struct SpotLight                                                                            
{                                                                                           
    PointLight Base;                                                                 
    vec3 Direction;                                                                         
    float Cutoff;                                                                           
};

// FXAA
const float gLumaThreshold = float(0.25);
const float gMulReduce = float(1 / 8);
const float gMinReduce = float(1 / 128);
const vec2	gTexelStep = vec2(1 / 1280, 1 / 720);
const float gMaxSpan = float(8);
uniform int gAliasFlags;

uniform int gNumPointLights;                                                                
uniform int gNumSpotLights;                                                                 
uniform DirectionalLight gDirectionalLight;                                                 
uniform PointLight gPointLights[MAX_POINT_LIGHTS];                                          
uniform SpotLight gSpotLights[MAX_SPOT_LIGHTS];                                             
uniform sampler2D gColorMap;                                                                
uniform vec3 gEyeWorldPos;                                                                  
uniform float gMatSpecularIntensity;                                                        
uniform float gSpecularPower;


                                                                                            
vec4 CalcLightInternal(BaseLight Light, vec3 LightDirection, vec3 Normal)            
{                                                                                           
    vec4 AmbientColor = vec4(Light.Color * Light.AmbientIntensity, 1.0f);
    float DiffuseFactor = dot(Normal, -LightDirection);                                     
                                                                                            
    vec4 DiffuseColor  = vec4(0, 0, 0, 0);                                                  
    vec4 SpecularColor = vec4(0, 0, 0, 0);                                                  
                                                                                            
    if (DiffuseFactor > 0) {                                                                
        DiffuseColor = vec4(Light.Color * Light.DiffuseIntensity * DiffuseFactor, 1.0f);
                                                                                            
        vec3 VertexToEye = normalize(gEyeWorldPos - WorldPos_FS_in);                        
        vec3 LightReflect = normalize(reflect(LightDirection, Normal));                     
        float SpecularFactor = dot(VertexToEye, LightReflect);                              
        if (SpecularFactor > 0) {                                                           
            SpecularFactor = pow(SpecularFactor, gSpecularPower);                               
            SpecularColor = vec4(Light.Color * gMatSpecularIntensity * SpecularFactor, 1.0f);
        }                                                                                   
    }                                                                                       
                                                                                            
    return (AmbientColor + DiffuseColor + SpecularColor);                                   
}                                                                                           
                                                                                            
vec4 CalcDirectionalLight(vec3 Normal)                                                      
{                                                                                           
    return CalcLightInternal(gDirectionalLight.Base, gDirectionalLight.Direction, Normal);  
}                                                                                           
                                                                                            
vec4 CalcPointLight(PointLight l, vec3 Normal)                                       
{                                                                                           
    vec3 LightDirection = WorldPos_FS_in - l.Position;                                      
    float Distance = length(LightDirection);                                                
    LightDirection = normalize(LightDirection);                                             
                                                                                            
    vec4 Color = CalcLightInternal(l.Base, LightDirection, Normal);                         
    float Attenuation =  l.Atten.Constant +                                                 
                         l.Atten.Linear * Distance +                                        
                         l.Atten.Exp * Distance * Distance;                                 
                                                                                            
    return Color / Attenuation;                                                             
}                                                                                           
                                                                                            
vec4 CalcSpotLight(SpotLight l, vec3 Normal)                                         
{                                                                                           
    vec3 LightToPixel = normalize(WorldPos_FS_in - l.Base.Position);                        
    float SpotFactor = dot(LightToPixel, l.Direction);                                      
                                                                                            
    if (SpotFactor > l.Cutoff) {                                                            
        vec4 Color = CalcPointLight(l.Base, Normal);                                        
        return Color * (1.0 - (1.0 - SpotFactor) * 1.0/(1.0 - l.Cutoff));                   
    }                                                                                       
    else {                                                                                  
        return vec4(0,0,0,0);                                                               
    }                                                                                       
}


vec4 AliasedSample(sampler2D ColorMap, vec2 TexCoord, int Flags) {
	vec3 rgbM = texture(ColorMap, TexCoord).rgb;

	if (Flags == 0) {
		return vec4(rgbM, 1.0);
	}
	else if (Flags == 0x11) {
		rgbM = normalize(rgbM);
	}
	// Sampling neighbour texels. Offsets are adapted to OpenGL texture coordinates. 
	vec3 rgbNW = textureOffset(ColorMap, TexCoord, ivec2(-1,  1)).rgb;
	vec3 rgbNE = textureOffset(ColorMap, TexCoord, ivec2( 1,  1)).rgb;
	vec3 rgbSW = textureOffset(ColorMap, TexCoord, ivec2(-1, -1)).rgb;
	vec3 rgbSE = textureOffset(ColorMap, TexCoord, ivec2( 1, -1)).rgb;

	// see http://en.wikipedia.org/wiki/Grayscale
	const vec3 toLuma = vec3(0.299, 0.587, 0.114);

	// Convert from RGB to luma.
	float lumaNW = dot(rgbNW, toLuma);
	float lumaNE = dot(rgbNE, toLuma);
	float lumaSW = dot(rgbSW, toLuma);
	float lumaSE = dot(rgbSE, toLuma);
	float lumaM	 = dot(rgbM,  toLuma);

	// Gather minimum and maximum luma.
	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

	if (lumaMax - lumaMin < lumaMax * gLumaThreshold)
	{
		return vec4(rgbM, 1.0);
	}

	// Sampling is done along the gradient.
	vec2 samplingDirection;
	samplingDirection.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
	samplingDirection.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

	// Sampling step distance depends on the luma: The brighter the sampled texels, the smaller the final sampling step direction.
	// This results, that brighter areas are less blurred/more sharper than dark areas.  
	float samplingDirectionReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * 0.25 * gMulReduce, gMinReduce);

	// Factor for norming the sampling direction plus adding the brightness influence. 
	float minSamplingDirectionFactor = 1.0 / (min(abs(samplingDirection.x), abs(samplingDirection.y)) + samplingDirectionReduce); 
	
	// Calculate final sampling direction vector by reducing, clamping to a range and finally adapting to the texture size. 
	samplingDirection = clamp(samplingDirection * minSamplingDirectionFactor, vec2(-gMaxSpan, -gMaxSpan), vec2(gMaxSpan, gMaxSpan)) * gTexelStep;

	// Inner samples on the tab.
	vec3 rgbSampleNeg = texture(gColorMap, TexCoord_FS_in + samplingDirection * (1.0 / 3.0 - 0.5)).rgb;
	vec3 rgbSamplePos = texture(gColorMap, TexCoord_FS_in + samplingDirection * (2.0 / 3.0 - 0.5)).rgb;

	vec3 rgbTwoTab = (rgbSamplePos + rgbSampleNeg) * 0.5;

	// Outer samples on the tab.
	vec3 rgbSampleNegOuter = texture(gColorMap, TexCoord_FS_in + samplingDirection * (0.0 / 3.0 - 0.5)).rgb;
	vec3 rgbSamplePosOuter = texture(gColorMap, TexCoord_FS_in + samplingDirection * (3.0 / 3.0 - 0.5)).rgb;
	vec3 rgbFourTab = (rgbSamplePosOuter + rgbSampleNegOuter) * 0.25 + rgbTwoTab * 0.5;

	// Calculate luma for checking against the minimum and maximum value.
	float lumaFourTab = dot(rgbFourTab, toLuma);

	
	// Are outer samples of the tab beyond the edge ... 
	if (lumaFourTab < lumaMin || lumaFourTab > lumaMax)
	{
		if (Flags == 0x11) {
			return vec4(1.0, rgbTwoTab.gb, 1.0);
		}
		// ... yes, so use only two samples.
		return vec4(rgbTwoTab, 1.0);
	}
	else
	{
		if (Flags == 0x11) {
			return vec4(rgbFourTab.r, 1.0, rgbFourTab.b, 1.0);
		}
		// ... no, so use four samples. 
		return vec4(rgbFourTab, 1.0);
	}
}

void main()                                                                                 
{                                                                                           
    vec3 Normal = normalize(Normal_FS_in);                                                  
    vec4 TotalLight = CalcDirectionalLight(Normal);                                         
                                                                                            
    for (int i = 0 ; i < gNumPointLights ; i++) {                                           
        TotalLight += CalcPointLight(gPointLights[i], Normal);                              
    }                                                                                       
                                                                                            
    for (int i = 0 ; i < gNumSpotLights ; i++) {                                            
        TotalLight += CalcSpotLight(gSpotLights[i], Normal);                                
    }                                                                                       
                                                                                            
    //FragColor = texture(gColorMap, TexCoord_FS_in.xy) * TotalLight;                         
	FragColor = AliasedSample(gColorMap, TexCoord_FS_in, gAliasFlags) * TotalLight;
}