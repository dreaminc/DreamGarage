// cubemapConvolution.vert
// shadertype=glsl

layout (location = 0) in vec4 inV_vec4Position;
layout (location = 1) in vec4 inV_vec4Color;

out Data {
	vec4 color;
	vec3 position;
	vec4 vertWorldSpace;
	vec4 vertViewSpace;
} DataOut;

uniform mat4 u_mat4Model;
uniform mat4 u_mat4View;
uniform mat4 u_mat4Projection;
uniform mat4 u_mat4ModelView;
uniform mat4 u_mat4ViewProjection;
uniform mat4 u_mat4ViewOrientation;

// TODO: Move to CPU side
mat4 g_mat4ModelView = u_mat4View * u_mat4Model;
mat4 g_mat4InvTransposeModelView = transpose(inverse(g_mat4ModelView));

void main(void) {	
	vec4 vertWorldSpace = u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);
	vec4 vertViewSpace = u_mat4View * u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);

	DataOut.vertWorldSpace = vertWorldSpace;
	DataOut.vertViewSpace = vertViewSpace;
	DataOut.color = inV_vec4Color;

	// Projected Vert Position
	vec4 transformedPosition = u_mat4Projection * u_mat4ViewOrientation * vec4(inV_vec4Position.xyz, 1.0f);
	
	gl_Position = transformedPosition;
	
	DataOut.position = inV_vec4Position.xyz;
}