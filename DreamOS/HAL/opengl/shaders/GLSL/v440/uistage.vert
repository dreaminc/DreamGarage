// minimalTexture.vert
// shadertype=glsl

// This is a minimal texture shader that simply displays
// a vertex color and position (no lighting)

#version 440 core

layout (location = 0) in vec4 inV_vec4Position;
layout (location = 1) in vec4 inV_vec4Color;
layout (location = 2) in vec4 inV_vec4Normal;
layout (location = 3) in vec2 inV_vec2UVCoord;

out Data {
	vec4 color;
	vec2 uvCoord;
	//vec4 vertClipSpace;
	float angle;
} DataOut;

uniform mat4 u_mat4Model;
uniform mat4 u_mat4ViewProjection;

//uniform mat4 u_mat4ClippingProjection;
uniform bool u_clippingEnabled;

uniform vec4 u_ptQuadCenter;
//uniform vec4 u_vQuadNormal;
//uniform float u_quadWidth;

uniform vec4 u_ptOrigin;
uniform vec4 u_vOrigin;
uniform float u_dot;

void main(void) {	
	// UV Coordinate
	DataOut.uvCoord = inV_vec2UVCoord;

	// Vert Color
	DataOut.color = inV_vec4Color;

	if(u_clippingEnabled == true) {
		vec3 dotOrigin = normalize(vec3(u_vOrigin.x, 0.0f, u_vOrigin.z));
		vec3 dotDir = normalize(vec3(u_ptQuadCenter.x - u_ptOrigin.x, 0.0f, u_ptQuadCenter.z - u_ptOrigin.z));
		DataOut.angle = dot(dotOrigin, dotDir);
	}
	else {
		DataOut.angle = 0.0f;
	}

	// Projected Vert Position
	gl_Position = u_mat4ViewProjection * u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);
}