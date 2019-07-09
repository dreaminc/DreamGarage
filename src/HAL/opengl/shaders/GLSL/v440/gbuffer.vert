// gbuffer.vert
// shadertype=glsl

// The G-Buffer stage for the use of SSAO / deferred rendering effects

layout (location = 0) in vec4 inV_vec4Position;
layout (location = 1) in vec4 inV_vec4Color;
layout (location = 2) in vec4 inV_vec4Normal;
layout (location = 3) in vec2 inV_vec2UVCoord;

out Data {
	vec4 color;
	vec2 uvCoord;
	vec4 vertWorldSpace;
	vec3 vWorldNormal;
} DataOut;

uniform mat4 u_mat4Model;
uniform mat4 u_mat4ViewProjection;

void main(void) {	

	vec4 vertWorldSpace = u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);

	// UV Coordinate
	DataOut.uvCoord = inV_vec2UVCoord;

	// Vert Color
	DataOut.color = inV_vec4Color;

	DataOut.vertWorldSpace = vertWorldSpace;
	DataOut.vWorldNormal = (u_mat4Model * vec4(inV_vec4Normal.xyz, 0.0f)).xyz;

	// Projected Vert Position
	gl_Position = u_mat4ViewProjection * u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);
}