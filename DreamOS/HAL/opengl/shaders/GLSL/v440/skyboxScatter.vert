// minimal.vert
// shadertype=glsl

// This is a minimal shader that simply displays
// a vertex color and position (no lighting, no textures)

#version 440 core

layout (location = 0) in vec4 inV_vec4Position;
layout (location = 1) in vec4 inV_vec4Color;

out Data {
	vec4 color;
	mat4 invProjection;
	mat4 invViewOrientation;
//	vec2 viewport;
} DataOut;

uniform mat4 u_mat4Model;
uniform mat4 u_mat4View;
uniform mat4 u_mat4Projection;
uniform mat4 u_mat4ViewOrientation;
//uniform mat4 u_mat4ViewProjection;




void main(void) {	
	// Vert Color
	DataOut.color = inV_vec4Color;

	DataOut.invProjection = inverse(u_mat4Projection);
	//need view rotation matrix and inverse
	DataOut.invViewOrientation = inverse(u_mat4ViewOrientation);
	//DataOut.viewport = vec2(1332, 1586);
//	DataOut.viewport = ivec2(1182.0f, 1464.0f);
	//DataOut.viewport = vec2(u_pxWidth, u_pxHeight);

	// Projected Vert Position
	mat4 viewProjection = u_mat4Projection * u_mat4View;
	gl_Position = viewProjection * u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);
//	gl_Position = u_mat4ViewProjection * u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);
}
