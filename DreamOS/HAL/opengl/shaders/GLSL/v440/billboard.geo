// billboard.geo
// shadertype=glsl

layout (points) in; // one point (billboard center)
layout (line_strip, max_vertices = 4) out; // four points (billboard quad)

uniform float u_width;
uniform float u_height;

uniform vec4 u_vec4CameraOrigin;

out GS_OUT {
	vec2 uvCoord;
} gs_out;

void GenerateQuad() {

	vec4 ptOrigin = gl_in[0].gl_Position;

	vec4 vPlaneNormal = ptOrigin - u_vec4CameraOrigin;
	vec4 vXAxis = vec4(1.0, 0.0, 0.0, 1.0);
	vec4 vYAxis = vec4(0.0, 1.0, 0.0, 1.0);

	vec4 vXNormal = vec4(cross(vPlaneNormal.xyz, vYAxis.xyz), 1.0);
	vec4 vYNormal = vec4(cross(vPlaneNormal.xyz, vYAxis.xyz), 1.0);

	//TODO: scale u_width, u_height based on vPlaneNormal magnitude
	vec4 xDiff = u_width/2.0f * (vXNormal);
	vec4 yDiff = u_height/2.0f * (vYNormal);

	gl_Position = ptOrigin + xDiff + yDiff;
	gs_out.uvCoord = vec2(1.0,1.0);
	EmitVertex();

	gl_Position = ptOrigin - xDiff + yDiff;
	gs_out.uvCoord = vec2(0.0,1.0);
	EmitVertex();

	gl_Position = ptOrigin + xDiff - yDiff;
	gs_out.uvCoord = vec2(1.0,0.0);
	EmitVertex();

	gl_Position = ptOrigin - xDiff - yDiff;
	gs_out.uvCoord = vec2(0.0,0.0);
	EmitVertex();

	EndPrimitive();

}

void main(void) {
	GenerateQuad();
}