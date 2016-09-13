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

	// Projected Vert Position
	float xoffset = u_mat4Projection[2][0]*-1;
	float yoffset = u_mat4Projection[2][1]*-1;

	// Projected Vert Position
	vec4 transformedPosition = u_mat4Model * vec4(inV_vec4Position.x + xoffset, inV_vec4Position.y + yoffset, inV_vec4Position.z, 1.0f);
	float z = clamp(inV_vec4Position.z, -0.99f, 0.99f);
	gl_Position = vec4(transformedPosition.xy, z, transformedPosition.w);
	gl_Position = u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);
}
