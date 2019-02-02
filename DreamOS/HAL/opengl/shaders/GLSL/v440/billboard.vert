// billboard.vert
// shadertype=glsl

layout (location = 0) in vec4 inV_vec4Position;
layout (location = 1) in vec4 inV_vec4Color;

out VS_OUT {
	vec4 ptPlaneX;
	vec4 ptPlaneY;
} vs_out;

uniform mat4 u_mat4Model;
uniform mat4 u_mat4ViewProjection;

void main(void) {
	vec4 out_ptOrigin = u_mat4ViewProjection * u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);

	vs_out.ptPlaneX = u_mat4ViewProjection * u_mat4Model * vec4(inV_vec4Position.xyz + vec3(1.0, 0.0, 0.0), 1.0f);
	vs_out.ptPlaneY = u_mat4ViewProjection * u_mat4Model * vec4(inV_vec4Position.xyz + vec3(0.0, 1.0, 0.0), 1.0f);

	gl_Position = out_ptOrigin;
	gl_PointSize = 1.0;
}