// AACommon.shader
// shadertype=glsl

// A shared GLSL library for Anti-Aliasing Functions

#define EDGE_THRESHOLD_MIN 0.0312f
#define EDGE_THRESHOLD_MAX 0.125f
#define MAX_FXAA_ITERATIONS 12
#define FXAA_SUBPIXEL_QUALITY 0.75f

float RGBToLuma(vec3 vec3RGBColor) {
	return sqrt(dot(vec3RGBColor, vec3(0.299f, 0.587f, 0.114f)));
}

float FXAAQUALITY(int i) {
	if (i > 5) {
		switch (i) {
		case 6: 1.5f; break;
		case 7: 2.0f; break;
		case 8: 2.0f; break;
		case 9: 2.0f; break;
		case 10: 2.0f; break;
		case 11: 4.0f; break;
		case 12: 8.0f; break;
		}
	}

	return 1.0f;
}

vec4 GetFXAAColor(sampler2D screenBufferTexture, vec2 uvCoord, vec2 inverseScreenSize) {
	vec4 retColor = texture(screenBufferTexture, uvCoord);

	vec3 colorCenter = retColor.rgb;

	// Luma at the current fragment
	float lumaCenter = RGBToLuma(colorCenter);

	// Luma at the four direct neighbours of the current fragment
	float lumaDown = RGBToLuma(textureOffset(screenBufferTexture, uvCoord, ivec2(0.0f, -1.0f)).rgb);
	float lumaUp = RGBToLuma(textureOffset(screenBufferTexture, uvCoord, ivec2(0.0f, 1.0f)).rgb);
	float lumaLeft = RGBToLuma(textureOffset(screenBufferTexture, uvCoord, ivec2(-1.0f, 0.0f)).rgb);
	float lumaRight = RGBToLuma(textureOffset(screenBufferTexture, uvCoord, ivec2(1.0f, 0.0f)).rgb);

	// Find the maximum and minimum luma around the current fragment
	float lumaMin = min(lumaCenter, min(min(lumaDown, lumaUp), min(lumaLeft, lumaRight)));
	float lumaMax = max(lumaCenter, max(max(lumaDown, lumaUp), max(lumaLeft, lumaRight)));

	// Compute the range
	float lumaRange = lumaMax - lumaMin;

	// If the luma variation is lower that a threshold (or if we are in a really dark area), 
	// we are not on an edge, don't perform any AA
	if (lumaRange < max(EDGE_THRESHOLD_MIN, lumaMax * EDGE_THRESHOLD_MAX)) {
		return retColor;
	}

	// Query the 4 remaining corners lumas
	float lumaDownLeft = RGBToLuma(textureOffset(screenBufferTexture, uvCoord, ivec2(-1.0f, -1.0f)).rgb);
	float lumaUpRight = RGBToLuma(textureOffset(screenBufferTexture, uvCoord, ivec2(1.0f, 1.0f)).rgb);
	float lumaUpLeft = RGBToLuma(textureOffset(screenBufferTexture, uvCoord, ivec2(-1.0f, 1.0f)).rgb);
	float lumaDownRight = RGBToLuma(textureOffset(screenBufferTexture, uvCoord, ivec2(1.0f, -1.0f)).rgb);

	// Combine the four edges lumas (using intermediary variables for future computations with the same values)
	float lumaDownUp = lumaDown + lumaUp;
	float lumaLeftRight = lumaLeft + lumaRight;

	// Same for corners
	float lumaLeftCorners = lumaDownLeft + lumaUpLeft;
	float lumaDownCorners = lumaDownLeft + lumaDownRight;
	float lumaRightCorners = lumaDownRight + lumaUpRight;
	float lumaUpCorners = lumaUpRight + lumaUpLeft;

	// Compute an estimation of the gradient along the horizontal and vertical axis.
	float edgeHorizontal = abs(-2.0f * lumaLeft + lumaLeftCorners) +
		abs(-2.0f * lumaCenter + lumaDownUp) * 2.0f +
		abs(-2.0f * lumaRight + lumaRightCorners);

	float edgeVertical = abs(-2.0f * lumaUp + lumaUpCorners) +
		abs(-2.0f * lumaCenter + lumaLeftRight) * 2.0f +
		abs(-2.0f * lumaDown + lumaDownCorners);

	// Is the local edge horizontal or vertical ?
	bool fHorizontal = (edgeHorizontal >= edgeVertical);

	// Select the two neighboring texels lumas in the opposite direction to the local edge
	float luma1 = fHorizontal ? lumaDown : lumaLeft;
	float luma2 = fHorizontal ? lumaUp : lumaRight;

	// Compute gradients in this direction
	float gradient1 = luma1 - lumaCenter;
	float gradient2 = luma2 - lumaCenter;

	// Which direction is the steepest ?
	bool f1Steepest = abs(gradient1) >= abs(gradient2);

	// Gradient in the corresponding direction, normalized
	float gradientScaled = 0.25f * max(abs(gradient1), abs(gradient2));

	// Choose the step size (one pixel) according to the edge direction
	float stepLength = fHorizontal ? inverseScreenSize.y : inverseScreenSize.x;

	// Average luma in the correct direction.
	float lumaLocalAverage = 0.0f;

	if (f1Steepest) {
		// Switch the direction
		stepLength = -stepLength;
		lumaLocalAverage = 0.5f * (luma1 + lumaCenter);
	}
	else {
		lumaLocalAverage = 0.5f * (luma2 + lumaCenter);
	}

	// Shift UV in the correct direction by half a pixel
	vec2 curUVCoord = uvCoord;

	if (fHorizontal) {
		curUVCoord.y += stepLength * 0.5f;
	}
	else {
		curUVCoord.x += stepLength * 0.5f;
	}

	// Compute offset (for each iteration step) in the right direction
	vec2 offset = fHorizontal ? vec2(inverseScreenSize.x, 0.0f) : vec2(0.0f, inverseScreenSize.y);

	// Compute UVs to explore on each side of the edge, orthogonally. The QUALITY allows us to step faster.
	vec2 uv1 = curUVCoord - offset;
	vec2 uv2 = curUVCoord + offset;

	// Read the lumas at both current extremities of the exploration segment, and compute the delta wrt to the local average luma
	float lumaEnd1 = RGBToLuma(texture(screenBufferTexture, uv1).rgb);
	float lumaEnd2 = RGBToLuma(texture(screenBufferTexture, uv2).rgb);
	lumaEnd1 -= lumaLocalAverage;
	lumaEnd2 -= lumaLocalAverage;

	// If the luma deltas at the current extremities are larger than the local gradient, we have reached the side of the edge
	bool fReached1 = abs(lumaEnd1) >= gradientScaled;
	bool fReached2 = abs(lumaEnd2) >= gradientScaled;
	bool fReachedBoth = fReached1 && fReached2;

	// If the side is not reached, we continue to explore in this direction
	if (fReached1 == false) {
		uv1 -= offset;
	}
	if (fReached2 == false) {
		uv2 += offset;
	}

	// If both sides have not been reached, continue to explore
	if (fReachedBoth == false) {

		for (int i = 2; i < MAX_FXAA_ITERATIONS; i++) {
			// If needed, read luma in 1st direction, compute delta.
			if (fReached1 == false) {
				lumaEnd1 = RGBToLuma(texture(screenBufferTexture, uv1).rgb);
				lumaEnd1 = lumaEnd1 - lumaLocalAverage;
			}
			// If needed, read luma in opposite direction, compute delta.
			if (fReached2 == false) {
				lumaEnd2 = RGBToLuma(texture(screenBufferTexture, uv2).rgb);
				lumaEnd2 = lumaEnd2 - lumaLocalAverage;
			}
			// If the luma deltas at the current extremities is larger than the local gradient, we have reached the side of the edge.
			fReached1 = abs(lumaEnd1) >= gradientScaled;
			fReached2 = abs(lumaEnd2) >= gradientScaled;
			fReachedBoth = fReached1 && fReached2;

			// If the side is not reached, we continue to explore in this direction, with a variable quality.
			if (fReached1 == false) {
				uv1 -= offset * FXAAQUALITY(i);
				//uv1 -= offset;
			}
			if (fReached2 == false) {
				uv2 += offset * FXAAQUALITY(i);
				//uv2 += offset;
			}

			// If both sides have been reached, stop the exploration.
			if (fReachedBoth) {
				break;
			}
		}
	}

	// Compute the distances to each extremity of the edge
	float distance1 = fHorizontal ? (uvCoord.x - uv1.x) : (uvCoord.y - uv1.y);
	float distance2 = fHorizontal ? (uv2.x - uvCoord.x) : (uv2.y - uvCoord.y);

	// In which direction is the extremity of the edge closer ?
	bool fDirection1 = distance1 < distance2;
	float distanceFinal = min(distance1, distance2);

	// Length of the edge.
	float edgeThickness = (distance1 + distance2);

	// UV offset: read in the direction of the closest side of the edge
	float pixelOffset = -distanceFinal / edgeThickness + 0.5f;

	// Is the luma at center smaller than the local average ?
	bool fLumaCenterSmaller = lumaCenter < lumaLocalAverage;

	// If the luma at center is smaller than at its neighbour, the delta luma at each end should be positive (same variation).
	// (in the direction of the closer side of the edge.)
	bool fCorrectVariation = ((fDirection1 ? lumaEnd1 : lumaEnd2) < 0.0) != fLumaCenterSmaller;

	// If the luma variation is incorrect, do not offset.
	float finalOffset = fCorrectVariation ? pixelOffset : 0.0;

	/*
	// Sub-pixel shifting
	// Full weighted average of the luma over the 3x3 neighborhood
	float lumaAverage = (1.0f / 12.0f) * (2.0f * (lumaDownUp + lumaLeftRight) + lumaLeftCorners + lumaRightCorners);

	// Ratio of the delta between the global average and the center luma, over the luma range in the 3x3 neighborhood.
	float subPixelOffset1 = clamp(abs(lumaAverage - lumaCenter) / lumaRange, 0.0f, 1.0f);
	float subPixelOffset2 = (-2.0f * subPixelOffset1 + 3.0f) * subPixelOffset1 * subPixelOffset1;

	// Compute a sub-pixel offset based on this delta.
	float subPixelOffsetFinal = subPixelOffset2 * subPixelOffset2 * FXAA_SUBPIXEL_QUALITY;

	// Pick the biggest of the two offsets
	finalOffset = max(finalOffset, subPixelOffsetFinal);
	//*/

	// Compute the final UV coordinates
	vec2 finalUv = uvCoord;

	if (fHorizontal) {
		finalUv.y += finalOffset * stepLength;
	}
	else {
		finalUv.x += finalOffset * stepLength;
	}

	// Read the color at the new UV coordinates, and use it.
	retColor = texture(screenBufferTexture, finalUv);

	return retColor;
}