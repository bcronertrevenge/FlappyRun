#version 410 core

#define POSITION	0
#define NORMAL		1
#define TEXCOORD	2
#define FRAG_COLOR	0

precision highp int;

uniform float Time;
uniform sampler2D Diffuse;
uniform sampler2D Specular;
uniform vec3 Light;
uniform float SpecularPower;

uniform vec3 PointLightPositions[12];
uniform vec3 PointLightColor;
uniform float PointLightIntensity;

uniform vec3 DirectionalLightDirection;
uniform vec3 DirectionalLightColor;
uniform float DirectionalLightIntensity;

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
		float att = 1.f / (d*d);
		outColor += att * PointLightColor * PointLightIntensity * (diffuseColor * ndotl + specularColor * pow(ndoth, SpecularPower));
	}
	return outColor;
}

vec3 directionalLights( in vec3 n, in vec3 v, in vec3 diffuseColor, in vec3 specularColor, in float specularPower)
{
	vec3 outColor = vec3(0);
	for (int i = 0; i < 1; ++i) {
		vec3 l = normalize(-DirectionalLightDirection);
		float ndotl = max(dot(n, l), 0.0);
		vec3 h = normalize(l+v);
		float ndoth = max(dot(n, h), 0.0);
		outColor += DirectionalLightColor * DirectionalLightIntensity * (diffuseColor * ndotl + specularColor * pow(ndoth, SpecularPower));
	}
	return outColor;
}


void main()
{
#define EX4
#if defined(EX1)
	//FragColor = vec4(1.0, 1.0, 0.0, 1.0);
	//FragColor = vec4(GeomTexcoord.s, GeomTexcoord.t, 0.0, 1.0);
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
	vec3 l = normalize(Light - In.CameraSpacePosition);
	float ndotl =  max(dot(n, l), 0.0);
	vec3 h = normalize(l-normalize(In.CameraSpacePosition));
	float ndoth = max(dot(n, h), 0.0);
	//vec3 color = texture(Diffuse, In.Texcoord).rgb * ndotl + texture(Specular, In.Texcoord).rrr * pow(ndoth, SpecularPower);
	vec3 color = pointLights( n, v, texture(Diffuse, In.Texcoord).rgb, texture(Specular, In.Texcoord).rrr, SpecularPower ) 
				+ directionalLights(n, v, texture(Diffuse, In.Texcoord).rgb, texture(Specular, In.Texcoord).rrr, SpecularPower);
	FragColor = vec4(color, 1.0);
	// FragColor = vec4(vec3(ndotl), 1.0);
	// FragColor = vec4(vec3(n), 1.0);
	// FragColor = vec4(vec3(l), 1.0);

#endif
}
