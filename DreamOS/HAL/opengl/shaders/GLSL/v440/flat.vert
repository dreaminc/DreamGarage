#version 440 core

layout (location = 0) in vec4 inV_vec4Position;
layout (location = 1) in vec4 inV_vec4Color;
layout (location = 3) in vec2 inV_vec2UVCoord;

out Data {
	vec4 color;
	vec2 uvCoord;
} DataOut;

uniform mat4 u_mat4Model;
uniform mat4 u_mat4Projection;

void main(void) {	
	// UV Coordinate
	DataOut.uvCoord = inV_vec2UVCoord;

	// Vert Color
	DataOut.color = inV_vec4Color;

	/*
	// Projected Vert Position
	float xoffset = u_mat4Projection[2][0] * -1.0f;
	float yoffset = u_mat4Projection[2][1] * -1.0f;

	// Projected Vert Position
	gl_Position = u_mat4Model * vec4(inV_vec4Position.x + xoffset, inV_vec4Position.y + yoffset, inV_vec4Position.z, 1.0f);
	*/

	gl_Position = u_mat4Projection * u_mat4Model * inV_vec4Position;
}
