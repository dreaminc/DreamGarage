// visualize_normals.geo
// shadertype=glsl

// This is a minimal vertex shader for the visualization of normals program

layout (location = 0) in vec4 inV_vec4Position;
layout (location = 1) in vec4 inV_vec4Color;
layout (location = 2) in vec4 inV_vec4Normal;

out VS_OUT {
    vec3 vNormal;
} vs_out;

uniform mat4 u_mat4Model;
uniform mat4 u_mat4View;
uniform mat4 u_mat4Projection;
uniform mat4 u_mat4ViewProjection;

void main(void) {	
	// Projected Vert Position
	
	gl_Position = u_mat4ViewProjection * u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);

	mat3 normalMatrix = mat3(transpose(inverse(u_mat4View * u_mat4Model)));
	vs_out.vNormal = normalize(vec3(u_mat4Projection * vec4(normalMatrix * inV_vec4Normal.xyz, 0.0)));
}