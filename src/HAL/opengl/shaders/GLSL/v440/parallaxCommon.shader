// parallaxCommon.shader
// shadertype=glsl

// This is a shared GLSL library for the parallax occulsion 
// effect.

// To use the below ParallaxMapping function, the ParallaxMapping_HeightFunction
// needs to be defined in the shader using the code.  This allows the parallax 
// effect to calculate the height based on a texture, or whatever way the
// target shader wants

float ParallaxMapping_HeightFunction(vec2 uvCoord);

vec2 ParallaxMapping(in vec2 texCoords, in vec3 viewDir, in float heightScale) {

	// The steeper the angle the more layers
	const float minLayers = 8.0;
	const float maxLayers = 32.0;
	float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0f, 0.0f, 1.0f), viewDir)));

	float layerDepth = 1.0 / numLayers;
	float currentLayerDepth = 0.0;

	vec2 vPIncrement = viewDir.xy * heightScale;
	vec2 deltaTexCoords = vPIncrement / numLayers;

	// Initial values
	vec2 currentTexCoords = texCoords;
	float currentDepthMapValue = ParallaxMapping_HeightFunction(currentTexCoords);

	vec2 prevTexCoords;

	// Detect when we've gone "deeper" in the height map
	while (currentLayerDepth < currentDepthMapValue) {
		prevTexCoords = currentTexCoords;
		currentTexCoords -= deltaTexCoords;
		currentDepthMapValue = ParallaxMapping_HeightFunction(currentTexCoords);
		currentLayerDepth += layerDepth;
	}

	// perf?
	// Get texture coordinates before collision (reverse operations)
	// vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	// Get depth after and before collision for linear interpolation
	float afterDepth = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = ParallaxMapping_HeightFunction(prevTexCoords) - currentLayerDepth + layerDepth;

	// Interpolation of texture coordinates
	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return finalTexCoords;
}
