// mathCommon.shader
// shadertype=glsl

// A shared GLSL library for math functions

float sigmoid(float rate, float progress) {
	return (tanh(rate*(progress - 0.5f)) + 1.0f) / 2.0f;
}