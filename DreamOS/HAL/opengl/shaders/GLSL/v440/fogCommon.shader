// fogCommon.shader
// shadertype=glsl

// A shared GLSL library for calculating fog

// Fog Types
const uint FOG_TYPE_LINEAR = 1u;
const uint FOG_TYPE_EXPONENTIAL = 2u;
const uint FOG_TYPE_EXPONENTIAL_SQUARED = 3u;

struct FogConfiguration {
	float startDistance;
	float endDistance;
	float density;
	float whatTheFuck;
	vec4 color;
};

layout(std140) uniform ub_fogConfig{
	FogConfiguration fogConfig;
};

vec4 MixWithFog(in uint fogType, in vec4 shaderColor, in float fragmentDepth, in FogConfiguration fogConfiguration) {

	//fogConfiguration.startDistance = 50.0f;
	//fogConfiguration.endDistance = 300.0f;
	//fogConfiguration.density = 0.05;
	//fogConfiguration.color = vec4(222.0f / 255.0f, 222.0f / 255.0f, 222.0f / 255.0f, 1.0f);

	float fogFactor;

	switch (fogType) {
		case(FOG_TYPE_LINEAR): {
			float fogEndDistance = fogConfiguration.endDistance;
			float fogStartDistance = fogConfiguration.startDistance;
			fogFactor = 1.0f - ((fogEndDistance - fragmentDepth) / (fogEndDistance - fogStartDistance));
		} break;

		case(FOG_TYPE_EXPONENTIAL): {
			float fogDensity = fogConfiguration.density;
			fogFactor = 1.0f - exp(-(abs((fragmentDepth * fogDensity))));
		} break;

		case(FOG_TYPE_EXPONENTIAL_SQUARED): {
			float fogDensity = fogConfiguration.density;
			fogFactor = 1.0f - exp(-(pow((fragmentDepth * fogDensity), 2.0)));
		} break;
	}

	fogFactor = clamp(fogFactor, 0.0f, 1.0f);
	vec4 mixWithFogColor = vec4(mix(shaderColor.rgb, fogConfiguration.color.rgb, fogFactor), 1.0f);
	return mixWithFogColor;

}