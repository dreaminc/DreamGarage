// minimalTexture.vert
// shadertype=glsl

// This is a minimal texture shader that simply displays
// a vertex color and position (no lighting)

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
uniform mat4 u_mat4ViewProjection;

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
	//gl_Position = transformedPosition.xyww;
	gl_Position = vec4(transformedPosition.xy, 0.0f, 1.0f);
	//gl_Position = u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);
}
