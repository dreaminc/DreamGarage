// skyboxScatter.frag
// shadertype=glsl

// A scattering skybox fragment shader

#define PRIMARY_RAY_STEPS 16
#define SECONDARY_RAY_STEPS 8

#define PI 3.141592

vec2 RaySphereIntersection(vec3 ptRayOrigin, vec3 vRayDirection, float radius) {

	// returns roots for quadratic equation f(x) = ax^2 + bx + c
	float a = dot(vRayDirection, vRayDirection);
	float b = 2.0 * dot(vRayDirection, ptRayOrigin);
	float c = dot(ptRayOrigin, ptRayOrigin) - (radius * radius);
	float d = (b*b) - 4.0 * a * c;
	
	if (d < 0.0) return vec2(1e5, -1e5);

	return vec2(
		(-b - sqrt(d)) / (2.0*a),
		(-b + sqrt(d)) / (2.0*a)
	);
}

vec3 Atmosphere(vec3 vRayDirection,
	vec3 ptRayOrigin,
	vec3 ptSunOrigin,
	float sunIntensity,
	float radiusPlanet,
	float radiusAtmosphere,
	vec3 vRayleighScattering,
	float mieScattering,
	float rayleighScaleHeight,
	float mieScaleHeight,
	float mieScatteringDirection) 
{
	ptSunOrigin = normalize(ptSunOrigin);
	vRayDirection = normalize(vRayDirection);

	vec2 primaryIntersection = RaySphereIntersection(ptRayOrigin, vRayDirection, radiusAtmosphere);

	if (primaryIntersection.x > primaryIntersection.y) return vec3(0, 0, 0);

	float primaryIntersectionPlanetX = RaySphereIntersection(ptRayOrigin, vRayDirection, radiusPlanet).x;
	primaryIntersection.y = min(primaryIntersection.y, primaryIntersectionPlanetX);

	float primaryStepSize = (primaryIntersection.y - primaryIntersection.x) / float(PRIMARY_RAY_STEPS);

	float primaryRayTime = 0.0;

	vec3 accumulatedRayleigh = vec3(0, 0, 0);
	vec3 accumulatedMie = vec3(0, 0, 0);

	float accumulatedOpticalDepthRayleigh = 0.0;
	float accumulatedOpticalDepthMie = 0.0;

	float mu = dot(vRayDirection, ptSunOrigin);
	float muSquared = mu * mu;

	float gSquared = mieScatteringDirection * mieScatteringDirection;

	float phaseRayleigh = 3.0 / (16.0 * PI) * (1.0 + muSquared);
	float phaseMie = 3.0 / (8.0 * PI) * ((1.0 - gSquared) * (muSquared + 1.0)) / (pow(1.0 + gSquared - 2.0 * mu * mieScatteringDirection, 1.5) * (2.0 + gSquared));

	for (int i = 0; i < PRIMARY_RAY_STEPS; i++) {
		vec3 stepPosition = ptRayOrigin + vRayDirection * (primaryRayTime + primaryStepSize * 0.5);

		float stepHeight = length(stepPosition) - radiusPlanet;

		float odStepRayleigh = exp(-stepHeight / rayleighScaleHeight) * primaryStepSize;
		float odStepMie = exp(-stepHeight / mieScaleHeight) * primaryStepSize;

		accumulatedOpticalDepthRayleigh += odStepRayleigh;
		accumulatedOpticalDepthMie += odStepMie;

		float secondaryStepSize = RaySphereIntersection(stepPosition, ptSunOrigin, radiusAtmosphere).y / float(SECONDARY_RAY_STEPS);

		float secondaryTime = 0.0;

		float secondaryOpticalDepthRayleigh = 0.0;
		float secondaryOpticalDepthMie = 0.0;

		for (int j = 0; j < SECONDARY_RAY_STEPS; j++) {
			vec3 secondaryPosition = stepPosition + ptSunOrigin * (secondaryTime + secondaryStepSize * 0.5);

			float secondaryHeight = length(secondaryPosition) - radiusPlanet;

			secondaryOpticalDepthRayleigh += exp(-secondaryHeight / rayleighScaleHeight) * secondaryStepSize;
			secondaryOpticalDepthMie += exp(-secondaryHeight / mieScaleHeight) * secondaryStepSize;

			secondaryTime += secondaryStepSize;
		}

		vec3 attenuation = exp(-(mieScattering * (accumulatedOpticalDepthMie + secondaryOpticalDepthMie) + vRayleighScattering * (accumulatedOpticalDepthRayleigh + secondaryOpticalDepthRayleigh)));
		vec3 attenuationR = exp(-(vRayleighScattering * (accumulatedOpticalDepthRayleigh + secondaryOpticalDepthRayleigh)));
		vec3 attenuationM = vec3(1,1,1)*exp(-(mieScattering * (accumulatedOpticalDepthMie + secondaryOpticalDepthMie)));

		accumulatedRayleigh += odStepRayleigh * attenuation;
		accumulatedMie += odStepMie * attenuation;

		primaryRayTime += primaryStepSize;
	}

	return sunIntensity * (phaseRayleigh * vRayleighScattering * accumulatedRayleigh + phaseMie * mieScattering * accumulatedMie);
}

in Data {
	vec4 color;
	mat4 invProjection;
	mat4 invViewOrientation;
} DataIn;

uniform	int u_intViewWidth;
uniform int u_intViewHeight;
uniform vec4 u_vecSunDirection;

layout (location = 0) out vec4 out_vec4Color;

vec3 getWorldNormal() {

	vec2 fragCoord = gl_FragCoord.xy/ivec2(u_intViewWidth, u_intViewHeight);
	fragCoord = (fragCoord-0.5)*2.0;

	vec4 deviceNormal = vec4(fragCoord, 0.0f, 1.0f);
	vec4 eyeNormal = normalize((DataIn.invProjection * deviceNormal));
	vec3 worldNormal = normalize((DataIn.invViewOrientation*eyeNormal).xyz);

	return worldNormal;
}

void main(void) {  

	vec3 lightDirection = normalize(u_vecSunDirection.xyz);
	vec3 eyeDirection = getWorldNormal();

	//vec3 eyeOrigin = vec3(0,6371002,0);
	vec3 eyeOrigin = vec3(0,6372e3,0);
	float sunIntensity = 22.0;
	float radiusPlanet = 6371e3;
	float radiusAtmosphere = 6471e3;
	vec3 vRayleighScattering = vec3(5.5e-6,13.0e-6,22.4e-6);
	float mieScattering = 21e-6;
	float rayleighScaleHeight = 8e3;
	float mieScaleHeight = 1.7e3;
	float mieDirection = 0.9995;

	vec3 sunDirection = lightDirection;

	vec4 atmColor = vec4(Atmosphere(eyeDirection,
		eyeOrigin,
		sunDirection,
		sunIntensity,
		radiusPlanet,
		radiusAtmosphere,
		vRayleighScattering,
		mieScattering,
		rayleighScaleHeight,
		mieScaleHeight,
		mieDirection).xyz, 1.0);

	out_vec4Color = 1.0 - exp(-2.5*atmColor);
}
