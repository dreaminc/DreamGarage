// skyboxScatter.vert
// shadertype=glsl

// A scattering skybox vertex shader

layout (location = 0) in vec4 inV_vec4Position;
layout (location = 1) in vec4 inV_vec4Color;

out Data {
	vec4 color;
	mat4 invProjection;
	mat4 invViewOrientation;
} DataOut;

uniform mat4 u_mat4Model;
uniform mat4 u_mat4View;
uniform mat4 u_mat4Projection;
uniform mat4 u_mat4ViewOrientation;

void main(void) {	
	// Vert Color
	DataOut.color = inV_vec4Color;

	DataOut.invProjection = inverse(u_mat4Projection);

	// Need view rotation matrix and inverse
	DataOut.invViewOrientation = inverse(u_mat4ViewOrientation);

	mat4 viewProjection = u_mat4Projection * u_mat4View;

	gl_Position = viewProjection * u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);
}
