// skybox.vert
// shadertype=glsl

#version 440 core

layout (location = 0) in vec4 inV_vec4Position;
layout (location = 1) in vec4 inV_vec4Color;
layout (location = 2) in vec4 inV_vec4Normal;
layout (location = 3) in vec2 inV_vec2UVCoord;
layout (location = 4) in vec4 inV_vec4Tangent;
layout (location = 5) in vec4 inV_vec4Bitangent;

//out vec3 inF_vec3Color;

out Data {
	vec4 color;
	vec2 uvCoord;
	vec3 position;
	vec4 vertWorldSpace;
	vec4 vertViewSpace;
} DataOut;

uniform mat4 u_mat4Model;
uniform mat4 u_mat4View;
uniform mat4 u_mat4Projection;
uniform mat4 u_mat4ModelView;
uniform mat4 u_mat4ViewProjection;
uniform mat4 u_mat4ViewOrientation;

// TODO: Move to CPU side
mat4 g_mat4ModelView = u_mat4View * u_mat4Model;
mat4 g_mat4InvTransposeModelView = transpose(inverse(g_mat4ModelView));

/*
// TODO: Move to GLSL shared lib
// Note that a quat is (w, x, y, z) so w is actually .x and so indices are used instead
vec3 RotateVectorByQuaternion(vec4 q, vec3 v) {
	vec3 retVal = vec3(0.0f);

	retVal.x = v.x * (1.0f - 2.0f * ((q[2]*q[2]) + (q[3]*q[3]))) +
			   v.y * (2.0f * ((q[1]*q[2]) + (q[0]*q[3])))		 +
			   v.z * (2.0f * ((q[1]*q[3]) - (q[0]*q[2])));

	retVal.y = v.x * (2.0f * ((q[1]*q[2]) - (q[0]*q[3])))		 +
			   v.y * (1.0f - 2.0f * ((q[1]*q[1]) + (q[3]*q[3]))) +
			   v.z * (2.0f * ((q[2]*q[3]) + (q[0]*q[1])));

	retVal.z = v.x * (2.0f * ((q[1]*q[3]) + (q[0]*q[2]))) +
			   v.y * (2.0f * ((q[2]*q[3]) - (q[0]*q[1]))) +
			   v.z * (1.0f - 2.0f * ((q[1]*q[1]) + (q[2]*q[2])));

	return retVal;
}
*/

void main(void) {	
	vec4 vertWorldSpace = u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);
	vec4 vertViewSpace = u_mat4View * u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);

	DataOut.vertWorldSpace = vertWorldSpace;
	DataOut.vertViewSpace = vertViewSpace;
	DataOut.uvCoord = inV_vec2UVCoord;
	DataOut.color = inV_vec4Color;

	// Projected Vert Position
	//vec3 rotatedPoint = RotateVectorByQuaternion(u_vec4ViewOrientationQuaternion, inV_vec4Position.xyz);
	//vec3 rotatedPoint = RotateVectorByQuaternion(vec4(0.0f, 0.0f, 0.0f, -1.0f), inV_vec4Position.xyz);
	vec4 transformedPosition = u_mat4Projection * u_mat4ViewOrientation * vec4(inV_vec4Position.xyz, 1.0f);
	//vec4 transformedPosition = u_mat4Projection * vec4(rotatedPoint, 1.0f);
	gl_Position = transformedPosition;
	DataOut.position = inV_vec4Position.xyz;
}