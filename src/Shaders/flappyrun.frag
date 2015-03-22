#version 410 core

#define POSITION	0
#define NORMAL		1
#define TEXCOORD	2
#define FRAG_COLOR	0

const float TWOPI = 6.28318530718;
const float DEG2RAD = TWOPI / 360.0;

precision highp int;

uniform float Time;
uniform sampler2D Diffuse;
uniform sampler2D Specular;
uniform float SpecularPower;

uniform vec3 PointLightPositions[12];
uniform vec3 PointLightColor;
uniform float PointLightIntensity;

uniform vec3 DirectionalLightDirection;
uniform vec3 DirectionalLightColor;
uniform float DirectionalLightIntensity;

uniform vec3 SpotLightDirection;
uniform vec3 SpotLightPosition;
uniform float SpotLightAngle;
uniform float SpotLightPenumbraAngle;
uniform vec3 SpotLightColor;
uniform float SpotLightIntensity;

uniform mat4 WorldToLightScreen;
uniform sampler2D Shadow;

layout(location = FRAG_COLOR, index = 0) out vec4 FragColor;

in block
{
	vec2 Texcoord;
	vec3 CameraSpacePosition;
	vec3 CameraSpaceNormal;
} In; 

vec3 pointLights( in vec3 n, in vec3 v, in vec3 diffuseColor, in vec3 specularColor, in float specularPower)
{
	vec3 outColor = vec3(0);
	for (int i = 0; i < 12; ++i) {
		vec3 l = normalize(PointLightPositions[i] - In.CameraSpacePosition);
		float ndotl = max(dot(n, l), 0.0);
		vec3 h = normalize(l+v);
		float ndoth = max(dot(n, h), 0.0);
		float d = distance(PointLightPositions[i], In.CameraSpacePosition);
		float att = 1. / (d*d);
		outColor += att * PointLightColor * PointLightIntensity * (diffuseColor * ndotl + specularColor * pow(ndoth, SpecularPower));
	}
	return outColor;
}

vec3 directionalLights( in vec3 n, in vec3 v, in vec3 diffuseColor, in vec3 specularColor, in float specularPower)
{
	vec3 l = normalize(-DirectionalLightDirection);
	float ndotl = max(dot(n, l), 0.0);
	vec3 h = normalize(l+v);
	float ndoth = max(dot(n, h), 0.0);

	return DirectionalLightColor * DirectionalLightIntensity * (diffuseColor * ndotl + specularColor * pow(ndoth, SpecularPower));
}

vec3 spotLight( in vec3 p, in vec3 n, in vec3 v, in vec3 diffuseColor, in vec3 specularColor, in float specularPower)
{
	vec3 l = normalize(SpotLightPosition - p);
	float a = cos(SpotLightAngle * DEG2RAD);
	float pa = cos(SpotLightPenumbraAngle * DEG2RAD);
	float ndotl = max(dot(n, l), 0.0);
	float ldotd = dot(-l, normalize(SpotLightDirection));
	vec3 h = normalize(l+v);
	float ndoth = max(dot(n, h), 0.0);
	float fallof = clamp(pow( (ldotd - a) / (a-pa), 4), 0.0, 1.0);
	float d = distance(SpotLightPosition, p);
	float att = 1.f / (d*d);

	return att * fallof * SpotLightColor * SpotLightIntensity * (diffuseColor * ndotl + specularColor * pow(ndoth, specularPower));
}

void main()
{
#define EX4
#if defined(EX1)
	FragColor = vec4(In.CameraSpacePosition.s, In.CameraSpacePosition.t, 0.0, 1.0);
	FragColor = vec4(In.CameraSpaceNormal.s, In.CameraSpaceNormal.t, 0.0, 1.0);
	FragColor = vec4(In.Texcoord.s, In.Texcoord.t, 0.0, 1.0);
#endif
#if defined(EX2)
	vec3 color = vec3(0.26, 0.37, 0.85);
	float x = In.Texcoord.x * cos(Time) + In.Texcoord.y * sin(Time);
	float y = In.Texcoord.x * sin(Time) - In.Texcoord.y + cos(Time);

	x = x*50.0 - 20.0 ;
	y = y*50.0 - 20.0;
	y *= 3.14159;

	if (  ( int(float(abs(x - sin(y))))%6 < 5 ) )
	{	
		color = vec3(0.25 * sin(Time*2.0), 0.25 * sin(Time), 0.1 * cos(Time));
	}
	FragColor = vec4(color, 1.0);
#endif
#if defined(EX3)
	vec3 color = mix(texture(Diffuse, In.Texcoord).rgb, texture(Specular, In.Texcoord).rrr, 0.7); ;
	FragColor = vec4(color, 1.0);
#endif
#if defined(EX4)
	vec3 n = normalize(In.CameraSpaceNormal);
	vec3 v = normalize(-In.CameraSpacePosition);

	vec3 color = pointLights( n, v, texture(Diffuse, In.Texcoord).rgb, texture(Specular, In.Texcoord).rrr, SpecularPower ) 
				+ spotLight( In.CameraSpacePosition, n, v, texture(Diffuse, In.Texcoord).rgb, texture(Specular, In.Texcoord).rrr, SpecularPower);
	FragColor = vec4(color, 1.0);

		
	vec4 wlP = WorldToLightScreen * vec4(In.CameraSpacePosition, 1.0);
	vec3 lP = vec3(wlP/wlP.w) * 0.5 + 0.5;

	float shadowDepth = texture(Shadow, lP.xy).r;

	if (shadowDepth + 0.6 < lP.z)
	{
		FragColor = vec4(pointLights( n, v, texture(Diffuse, In.Texcoord).rgb, texture(Specular, In.Texcoord).rrr, SpecularPower ) , 1.0);
	}
	else
	{
		FragColor = vec4(color, 1.0);
	}
	
#endif
}
