// billboard.vert
// shadertype=glsl

layout (location = 0) in vec4 inV_vec4Position;
layout (location = 1) in vec4 inV_vec4Color;

uniform mat4 u_mat4Model;
uniform mat4 u_mat4ViewProjection;

void main(void) {
	vec4 out_ptOrigin = u_mat4ViewProjection * u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);

	gl_Position = out_ptOrigin;
	gl_PointSize = 1.0;
}