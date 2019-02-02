// billboard.geo
// shadertype=glsl

layout (points) in; // one point (center)
layout (line_strip, max_vertices = 4) out; 

uniform float u_width;
uniform float u_height;

in VS_OUT {
	vec4 ptOrigin;
	vec4 ptPlaneX;
	vec4 ptPlaneY;
} gs_in[];

out GS_OUT {
	//vec4 color;
	vec2 uvCoord;
} gs_out;

void GenerateQuad() {
//	VS_OUT in = gs_in[0];
	vec4 ptOrigin = gs_in[0].ptOrigin;
	vec4 ptPlaneX = gs_in[0].ptPlaneX;
	vec4 ptPlaneY = gs_in[0].ptPlaneY;

	vec4 xDiff = u_width/2.0f * (ptPlaneX-ptOrigin);
	vec4 yDiff = u_height/2.0f * (ptPlaneY-ptOrigin);

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

	EndPrimitive;
}

void main(void) {
	GenerateQuad();
}