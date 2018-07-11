// skyboxScatter.frag
// shadertype=glsl

// A scattering skybox fragment shader

//TODO: remove before push
//#version 440 core

#define PRIMARY_RAY_STEPS 16
#define SECONDARY_RAY_STEPS 8

#define PI 3.141592

vec2 RaySphereIntersection(vec3 ptRayOrigin, vec3 vRayDirection, float radius) {
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
	//return sunIntensity * (phaseRayleigh * vRayleighScattering * accumulatedRayleigh);// + phaseMie * mieScattering * accumulatedMie);
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

vec4 displayAsColor(vec3 vec) {
	return vec4(vec.x / 2.0f + 0.5f,
				vec.y / 2.0f + 0.5f,
				vec.z / 2.0f + 0.5f,
				1.0);
}

float phase(float theta, float g) {

	float n1 = 3.0f * (1.0f - (g * g));
	float d1 = 2.0f * (2.0f + (g * g));

	float n2 = 1.0f + (theta * theta);
	float d2 = pow(1.0f + (g * g) - (2.0f * g * theta), 1.5f);

	return (n1 / d1) * (n2 / d2);
}

float atmosphericDepth(vec3 position, vec3 dir) {

	float a = dot(dir, dir);
	float b = 2.0f * dot(dir, position);
	float c = dot(position, position) - 1.0;
	
	float det = b * b - 4.0f * a * c;
	float detSqrt = sqrt(det);
	float q = (-b - detSqrt) / 2.0;

	float t1 = c / q;

	return t1;
}

float horizonExtinction(vec3 position, vec3 dir, float radius) {
	dir = normalize(dir);

	float u = dot(dir, -position);

	if(u < 0.0f) {
		return 1.0f;
	}

	vec3 near = position + u * dir;

	if(length(near) < radius) {
		return 0.0f;
	}
	else {
		vec3 v2 = normalize(near) * radius - position;
		float diff = acos(dot(normalize(v2), dir));

		return smoothstep(0.0f, 1.0f, pow(diff * 2.0f, 3.0f));
	}
}

vec3 absorb(float dist, vec3 color, vec3 Kr, float factor) {
	return color - color * pow(Kr, vec3(factor / dist));
}


vec3 attenuate(float dist, vec3 color, vec3 Kr, float factor) {
	float radius = 1.0f;
	float denom = ((dist)/radius) + 1.0f;
	float att = 1.0f / (denom * denom);

	return (att) * color;
}

void main(void) {  

	vec3 lightDirection = normalize(u_vecSunDirection.xyz);
	vec3 eyeDirection = getWorldNormal();
	float theta = dot(eyeDirection, lightDirection);
	
	float rayleighBrightness = 1.0f;
	float mieBrightness = 4.0f;
	float spotBrightness = (cos(theta) < 0.0f) ? 0.0f : 1.0f;

	float rayleighFactor = phase(theta + 1.25f, 0.0f) * rayleighBrightness;
	float mieFactor = phase(theta, 0.9995f) * mieBrightness;

	float sunSize = 0.9995f;
	float spotFactor = smoothstep(0.0f, 15.0f, phase(theta, sunSize))*spotBrightness;

	float surfaceHeight = 0.85f;
	int stepCount = 15;
	vec3 eyePosition = vec3(0.0, surfaceHeight, 0.0);
	float eyeDepth = atmosphericDepth(eyePosition, eyeDirection);
	float stepLength = eyeDepth/float(stepCount);
 
	float eyeExtinction = horizonExtinction(eyePosition, eyeDirection, surfaceHeight-0.15);

	vec4 colorHorizon = vec4(0.9f, 0.9f, 0.9f, 1.0f);
	
	//light sky blue
	//vec4 colorSky = vec4(135.0f / 255.0f, 206.0f / 255.0f, 235.0f / 255.0f, 1.0f);
	//deep sky blue
	vec4 colorSky = vec4(0.0f / 255.0f, 191.0f / 255.0f, 235.0f / 255.0f, 1.0f);

	// absorption profile of Nitrogen
	vec3 Kr = vec3(
		0.18867780436772762f, 0.4978442963618773f, 0.6616065586417131f
	);
	//vec3 Kr = u_vecSkyColor;
	
	vec3 rayleighCollected = vec3(0.0f, 0.0f, 0.0f);
	vec3 mieCollected = vec3(0.0f, 0.0f, 0.0f);

	vec4 intensity = vec4(1.1f, 1.1f, 1.1f, 1.0f);
	float rayleighStrength = 0.0f;
	float mieStrength = 0.5f;
	float scatterStrength = 0.5f;

	// Loop through the eye ray, approximating scattering at each iteration
	for(int i = 0; i < stepCount; i++) {
		
		float sampleDistance = stepLength * float(i);

		vec3 position = eyePosition + eyeDirection.xyz * sampleDistance;		
		float extinction = horizonExtinction(position, lightDirection, surfaceHeight - 0.35f);
		float sampleDepth = atmosphericDepth(position, lightDirection);

		vec3 influxRayleigh = attenuate(sampleDepth, intensity.xyz, Kr, scatterStrength) * extinction;
		rayleighCollected += (attenuate(sampleDistance, Kr*influxRayleigh, Kr, rayleighStrength));

		vec3 influxMie = absorb(sampleDepth, intensity.xyz, Kr, scatterStrength) * extinction;
		mieCollected += (absorb(sampleDistance, influxMie, Kr, mieStrength));
	}	

	float rayleighCollectionPower = 1.0f;
	float mieCollectionPower = 1.0f;

	rayleighCollected = (rayleighCollected * eyeExtinction * pow(eyeDepth, rayleighCollectionPower)) / float(stepCount);
	mieCollected = (mieCollected * eyeExtinction * pow(eyeDepth, mieCollectionPower)) / float(stepCount);

	float ed = (eyeDirection.y + 1.0f) / 2.0f;
	vec4 testColor = (ed * colorSky) + ((1.0f - ed) * colorHorizon);
	float nm = clamp(1.0f - (mieFactor / 6.0f), 0.0f, 1.0f);
	
	testColor = nm * testColor;
	
	vec3 outColor = vec3(
		(spotFactor * mieCollected) +
		(mieFactor * mieCollected) +
		(testColor.xyz)
		//rayleighCollected * colorSky.xyz
	);

	out_vec4Color = vec4(outColor, 1.0f);

	vec3 eyeOrigin = vec3(0,6372e3,0);
	float sunIntensity = 22.0;
	float radiusPlanet = 6371e3;
	float radiusAtmosphere = 6471e3;
	vec3 vRayleighScattering = vec3(5.5e-6,13.0e-6,22.4e-6);
	//float mieScattering = 0e-6;
	float mieScattering = 21e-6;
	float rayleighScaleHeight = 8e3;
	float mieScaleHeight = 1.2e3;
	float mieDirection = 0.758;

	//vec3 sunDirection = vec3(0,0.1,-0.9);
	vec3 sunDirection = vec3(0,0.5,-0.5);

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

	//out_vec4Color = 1.0 - exp(-0.5*atmColor);
	out_vec4Color = 1.0 - exp(-2.5*atmColor);

	//out_vec4Color = vec4(nm, nm, nm, 1.0f);
	//out_vec4Color = vec4(miefactor, miefactor, miefactor, 1.0f);
	//out_vec4Color = vec4(spotFactor, spotFactor, spotFactor, 1.0f);
	//out_vec4Color = vec4(theta, theta, theta, 1.0);

	//out_vec4Color = displayAsColor(eyeDirection);
}
