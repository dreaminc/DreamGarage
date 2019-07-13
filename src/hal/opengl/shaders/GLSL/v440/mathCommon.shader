// mathCommon.shader
// shadertype=glsl

// A shared GLSL library for math functions

float sigmoid(float rate, float progress) {
	return (tanh(rate*(progress - 0.5f)) + 1.0f) / 2.0f;
}

// TODO: Move to GLSL shared lib
// Note that a quat is (w, x, y, z) so w is actually .x and so indices are used instead
vec3 RotateVectorByQuaternion(vec4 q, vec3 v) {
	vec3 retVal = vec3(0.0f);

	retVal.x = v.x * (1.0f - 2.0f * ((q[2]*q[2]) + (q[3]*q[3])))	+
	v.y * (2.0f * ((q[1]*q[2]) + (q[0]*q[3])))						+
	v.z * (2.0f * ((q[1]*q[3]) - (q[0]*q[2])));

	retVal.y = v.x * (2.0f * ((q[1]*q[2]) - (q[0]*q[3])))			+
	v.y * (1.0f - 2.0f * ((q[1]*q[1]) + (q[3]*q[3])))				+
	v.z * (2.0f * ((q[2]*q[3]) + (q[0]*q[1])));

	retVal.z = v.x * (2.0f * ((q[1]*q[3]) + (q[0]*q[2])))			+
	v.y * (2.0f * ((q[2]*q[3]) - (q[0]*q[1])))						+
	v.z * (1.0f - 2.0f * ((q[1]*q[1]) + (q[2]*q[2])));

	return retVal;
}