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
	vec4 ptMid;
} DataOut;

uniform mat4 u_mat4Model;
uniform mat4 u_mat4ViewProjection;

uniform bool u_clippingEnabled;

uniform vec4 u_ptQuadCenter;
uniform mat4 u_mat4ParentModel;

uniform vec4 u_ptOrigin;

void main(void) {	
	// UV Coordinate
	DataOut.uvCoord = inV_vec2UVCoord;

	// Vert Color
	DataOut.color = inV_vec4Color;

	if(u_clippingEnabled == true) {
		vec3 vOrigintoQuadCenter = normalize(u_ptOrigin.xyz - u_ptQuadCenter.xyz);

		vec3 vBitangent = normalize(cross(vec3(0.0f, 1.0f, 0.0f),vOrigintoQuadCenter));

		vec4 invModel = u_mat4ParentModel * vec4(inV_vec4Position.xyz, 1.0f);
		vec3 vVertextoCenter = invModel.xyz - u_ptQuadCenter.xyz;

		vec3 ptMid = (vBitangent * (dot(vVertextoCenter, vBitangent)));
		DataOut.ptMid = u_ptQuadCenter + (vec4(ptMid.xyz, 0.0f));// + (vec4(right, 0.0f) * ratio);
	}
	else {
		DataOut.ptMid = vec4(0.0f);
	}

	// Projected Vert Position
	gl_Position = u_mat4ViewProjection * u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);
}