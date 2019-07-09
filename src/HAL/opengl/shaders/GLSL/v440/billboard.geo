// billboard.geo
// shadertype=glsl

layout (points) in; // one point (billboard center)
layout (triangle_strip, max_vertices = 6) out; // two triangle quad

uniform float u_width;
uniform float u_height;

uniform vec4 u_vec4CameraOrigin;

out GS_OUT {
	vec2 uvCoord;
} gs_out;

void GenerateQuad() {

	vec4 ptOrigin = gl_in[0].gl_Position;

	vec3 vXAxis = vec3(1.0, 0.0, 0.0);
	vec3 vYAxis = vec3(0.0, 1.0, 0.0);

	//TODO: scale u_width, u_height based on vPlaneNormal magnitude
	vec3 x3Diff = u_width/2.0f * (vXAxis);
	vec3 y3Diff = u_height/2.0f * (vYAxis);

	vec4 xDiff = vec4(x3Diff, 0.0);
	vec4 yDiff = vec4(y3Diff, 0.0);

	// TODO: generalize primitive creation into a geoCommon.shader file
//*
	// Triangle 1
	gl_Position = ptOrigin + xDiff + yDiff;
	gs_out.uvCoord = vec2(1.0,1.0);
	EmitVertex();

	gl_Position = ptOrigin - xDiff + yDiff;
	gs_out.uvCoord = vec2(0.0,1.0);
	EmitVertex();

	gl_Position = ptOrigin - xDiff - yDiff;
	gs_out.uvCoord = vec2(0.0,0.0);
	EmitVertex();
	EndPrimitive();

	// Triangle 2
	gl_Position = ptOrigin + xDiff - yDiff;
	gs_out.uvCoord = vec2(1.0,0.0);
	EmitVertex();

	gl_Position = ptOrigin + xDiff + yDiff;
	gs_out.uvCoord = vec2(1.0,1.0);
	EmitVertex();

	gl_Position = ptOrigin - xDiff - yDiff;
	gs_out.uvCoord = vec2(0.0,0.0);
	EmitVertex();
	EndPrimitive();
}

void main(void) {
	GenerateQuad();
}
