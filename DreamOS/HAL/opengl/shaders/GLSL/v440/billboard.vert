// billboard.vert
// shadertype=glsl

layout (location = 0) in vec4 inV_vec4Position;

out VS_OUT {
	vec3 ptOrigin;
	vec3 ptPlaneX;
	vec3 ptPlaneY;
} vs_out;

uniform mat4 u_mat4Model;
uniform mat4 u_mat4ViewProjection;

void main(void) {
	vec3 out_ptOrigin = u_mat4ViewProjection * u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);

	gl_position = out_ptOrigin;

	vs_out.ptOrigin = out_ptOrigin;
	vs_out.ptPlaneX = u_mat4ViewProjection * u_mat4Model * vec4(inV_vec4Position.xyz + vec3(1.0, 0.0, 0.0), 1.0f);
	vs_out.ptPlaneY = u_mat4ViewProjection * u_mat4Model * vec4(inV_vec4Position.xyz + vec3(0.0, 1.0, 0.0), 1.0f);
}