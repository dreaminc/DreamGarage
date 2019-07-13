// noiseCommon.shader
// shadertype=glsl

// A shared GLSL library for noise generation functionality

#ifdef GL_ES
precision mediump float;
#endif

//uniform vec2 u_resolution;
//uniform vec2 u_mouse;
//uniform float u_time;

vec2 random2(vec2 st) {
	st = vec2(dot(st, vec2(127.1, 311.7)),
		dot(st, vec2(269.5, 183.3)));

	return -1.0 + 2.0 * fract(sin(st) * 43758.5453123);
}

// Value Noise by Inigo Quilez - iq/2013
// https://www.shadertoy.com/view/lsf3WH
float noise(vec2 st) {
	vec2 i = floor(st);
	vec2 f = fract(st);

	vec2 u = f * f * (3.0 - 2.0 * f);

	return mix(mix(dot(random2(i + vec2(0.0, 0.0)), f - vec2(0.0, 0.0)),
				   dot(random2(i + vec2(1.0, 0.0)), f - vec2(1.0, 0.0)), u.x),
			   mix(dot(random2(i + vec2(0.0, 1.0)), f - vec2(0.0, 1.0)),
				   dot(random2(i + vec2(1.0, 1.0)), f - vec2(1.0, 1.0)), u.x), u.y);
}

vec3 getNoiseNormal(vec2 st) {

	// This determines the delta for purposes of calculating the gradient
	float deltaSize = 0.1f;

	vec2 xPos = st;
	xPos.x += deltaSize;

	vec2 yPos = st;
	yPos.y += deltaSize;

	float value = noise(st) * 0.5f + 0.5f;
	float valueX = noise(xPos) * 0.5f + 0.5f;
	float valueY = noise(yPos) * 0.5f + 0.5f;

	float xNormal = (value - valueX) / deltaSize;
	float yNormal = (value - valueY) / deltaSize;
	float zNormal = 1.0;

	vec3 vNormal = normalize(vec3(-xNormal, -yNormal, zNormal));

	//vNormal = smoothstep(.15, .55, vNormal);

	return vNormal;
}