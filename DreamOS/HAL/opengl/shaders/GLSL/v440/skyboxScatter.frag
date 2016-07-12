// minimal.vert
// shadertype=glsl

// This is a minimal shader that simply displays
// a vertex color and position (no lighting, no textures)

#version 440 core

in Data {
	vec4 color;
	mat4 invProjection;
	mat4 invViewOrientation;
	vec2 viewport;
} DataIn;

layout (location = 0) out vec4 out_vec4Color;

vec3 getWorldNormal() {

	vec2 fragCoord = gl_FragCoord.xy/DataIn.viewport;
	fragCoord = (fragCoord-0.5)*2.0;

	vec4 deviceNormal = vec4(fragCoord, 0.0, 1.0);
	vec4 eyeNormal = normalize((DataIn.invProjection * deviceNormal));
	vec3 worldNormal = normalize((DataIn.invViewOrientation*eyeNormal).xyz);

	return worldNormal;
}

vec4 displayAsColor(vec3 vec) {
	return vec4(	vec.x/2+0.5,
					vec.y/2+0.5,
					vec.z/2+0.5,
					1.0);
}

float phase(float theta, float g) {

	float n1 = 3.0*(1.0-(g*g));
	float d1 = 2.0*(2.0+(g*g));

	float n2 = 1.0 + (theta*theta);
	float d2 = pow(1.0+(g*g)-(2.0*g*theta), 1.5);

	return (n1/d1)*(n2/d2);
}

float atmosphericDepth(vec3 position, vec3 dir) {
	float a = dot(dir, dir);
	float b = 2.0*dot(dir, position);
	float c = dot(position, position)-1.0;
	
	float det = b*b-4.0*a*c;
	float detSqrt = sqrt(det);
	float q = (-b - detSqrt)/2.0;

	float t1 = c/q;
	return t1;
}

float horizonExtinction(vec3 position, vec3 dir, float radius) {
	dir = normalize(dir);
	float u = dot(dir, -position);
	if(u<0.0) {
		return 1.0;
	}
	vec3 near = position + u*dir;
	if(length(near) < radius) {
		return 0.0;
	}
	else {
		vec3 v2 = normalize(near)*radius - position;
		float diff = acos(dot(normalize(v2), dir));
		return smoothstep(0.0, 1.0, pow(diff*2.0, 3.0));
	}
}

vec3 absorb(float dist, vec3 color, vec3 Kr, float factor) {
	return color-color*pow(Kr, vec3(factor/dist));
}


void main(void) {  
	//out_vec4Color = DataIn.color;

	//vec4 lightDirection = normalize(vec4(0.0, 1.0, 0.0, 1.0));
	vec3 lightDirection = normalize(vec3(0.0, 0.5, -0.5));
	vec3 eyeDirection = getWorldNormal();
	float theta = dot(eyeDirection, lightDirection);
	
	float rayleighBrightness = 1.0;
	float mieBrightness = 1.0;
	float spotBrightness = 1.0;
	float rayleighFactor = phase(theta, 0.0)*rayleighBrightness;
	float mieFactor = phase(theta, -0.9995)*mieBrightness;
	float spotFactor = smoothstep(0.0, 15.0, phase(theta, 0.9995))*spotBrightness;

	float surfaceHeight = 0.75;
	int stepCount = 15;
	vec3 eyePosition = vec3(0.0, surfaceHeight, 0.0);
	float eyeDepth = atmosphericDepth(eyePosition, eyeDirection);
	float stepLength = eyeDepth/float(stepCount);
 
	float eyeExtinction = horizonExtinction(eyePosition, eyeDirection, surfaceHeight-0.15);

	// absorption profile of Nitrogen
	vec3 Kr = vec3(
		0.18867780436772762, 0.4978442963618773, 0.6616065586417131
	);
	
//	Kr = vec3(50.0f/255.0f, 125.0f/255.0f, 235.0f/255.0f);


	vec3 rayleighCollected = vec3(0.0, 0.0, 0.0);
	vec3 mieCollected = vec3(0.0, 0.0, 0.0);

	vec4 intensity = vec4(1.0, 1.0, 1.0, 1.0);
	//vec4 intensity = vec4(0.5, 0.5, 0.5, 0.5);
	float rayleighStrength = 1.0f;
	float mieStrength = 0.5f;
	float scatterStrength = 0.5f;

	// loop through the eye ray, approximating at each step
	for(int i=0; i < stepCount; i++) {
		
		float sampleDistance = stepLength*float(i);
		vec3 position = eyePosition + eyeDirection.xyz*sampleDistance;		
		float extinction = horizonExtinction(position, lightDirection, surfaceHeight-0.35);
		float sampleDepth = atmosphericDepth(position, lightDirection);

		vec3 influx = absorb(sampleDepth, intensity.xyz, Kr, scatterStrength)*extinction;

		rayleighCollected += (absorb(sampleDistance, Kr*influx, Kr, rayleighStrength));
		mieCollected += (absorb(sampleDistance, influx, Kr, mieStrength));
	}	

	float rayleighCollectionPower = 1.0f;
	float mieCollectionPower = 1.0f;
	rayleighCollected = (rayleighCollected * eyeExtinction * pow(eyeDepth, rayleighCollectionPower))/float(stepCount);
	mieCollected = (mieCollected * eyeExtinction * pow(eyeDepth, mieCollectionPower))/float(stepCount);
///*
	out_vec4Color = vec4(
		spotFactor*mieCollected +
		mieFactor*mieCollected+
		rayleighFactor*rayleighCollected,
		1.0);
	//	*/
	//out_vec4Color = vec4(theta, theta, theta, 1.0);
	//out_vec4Color = displayAsColor(eyeDirection);
}
