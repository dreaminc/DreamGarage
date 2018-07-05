// skyboxScatter.frag
// shadertype=glsl

// A scattering skybox fragment shader

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

	//out_vec4Color = vec4(nm, nm, nm, 1.0f);
	//out_vec4Color = vec4(miefactor, miefactor, miefactor, 1.0f);
	//out_vec4Color = vec4(spotFactor, spotFactor, spotFactor, 1.0f);
	//out_vec4Color = vec4(theta, theta, theta, 1.0);

	//out_vec4Color = displayAsColor(eyeDirection);
}
