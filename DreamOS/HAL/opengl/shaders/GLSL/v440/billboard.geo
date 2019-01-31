// billboard.geo
// shadertype=glsl

layout (points) in; // one point (center)
layout (line_strip, max_vertices = 4) out; 

uniform float u_width;
uniform float u_height;

in VS_OUT {
	vec3 ptOrigin;
	vec3 ptPlaneX;
	vec3 ptPlaneY;
} gs_in[];

out GS_OUT {
	//vec4 color;
	vec2 uvCoord;
} gs_out;

void GenerateQuad() {
	VS_OUT in = gs_in[0];

	vec3 xDiff = u_width/2.0f * (in.ptPlaneX-in.ptOrigin);
	vec3 yDiff = u_height/2.0f * (in.ptPlaneY-in.ptOrigin);

	gl_position = in.ptOrigin + xDiff + yDiff;
	gs_out.uvCoord = vec2(1.0,1.0);
	EmitVertex();

	gl_position = in.ptOrigin - xDiff + yDiff;
	gs_out.uvCoord = vec2(0.0,1.0);
	EmitVertex();

	gl_position = in.ptOrigin + xDiff - yDiff;
	gs_out.uvCoord = vec2(1.0,0.0);
	EmitVertex();

	gl_position = in.ptOrigin - xDiff - yDiff;
	gs_out.uvCoord = vec2(0.0,0.0);
	EmitVertex();

	EndPrimitive;
}