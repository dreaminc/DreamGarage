// fogCommon.shader
// shadertype=glsl

// A shared GLSL library for calculating fog

// Fog Types
const uint FOG_TYPE_LINEAR = 1u;
const uint FOG_TYPE_EXPONENTIAL = 2u;
const uint FOG_TYPE_EXPONENTIAL_SQUARED = 3u;

struct FogParams {
	float startDistance;
	float endDistance;
	float density;
	float reserved0;
	vec4 color;
};

layout(std140) uniform ub_fogParams{
	FogParams fogParams;
};

vec4 MixWithFog(in uint fogType, in vec4 shaderColor, in float fragmentDepth) {

	float fogFactor;

	switch (fogType) {
		case(FOG_TYPE_LINEAR): {
			float fogEndDistance = fogParams.endDistance;
			float fogStartDistance = fogParams.startDistance;
			fogFactor = 1.0f - ((fogEndDistance - fragmentDepth) / (fogEndDistance - fogStartDistance));
		} break;

		case(FOG_TYPE_EXPONENTIAL): {
			float fogDensity = fogParams.density;
			fogFactor = 1.0f - exp(-(abs((fragmentDepth * fogDensity))));
		} break;

		case(FOG_TYPE_EXPONENTIAL_SQUARED): {
			float fogDensity = fogParams.density;
			fogFactor = 1.0f - exp(-(pow((fragmentDepth * fogDensity), 2.0)));
		} break;
	}

	fogFactor = clamp(fogFactor, 0.0f, 1.0f);
	vec4 mixWithFogColor = vec4(mix(shaderColor.rgb, fogParams.color.rgb, fogFactor), 1.0f);
	return mixWithFogColor;

}