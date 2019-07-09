// visualize_normals.geo
// shadertype=glsl

// This is a minimal geometry shader for the visualization of normals program

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
	vec3 vNormal;
} gs_in[];

// DEBUG: Start with a pass through 

const float MAGNITUDE = 0.25;

void GenerateLine(int index) {
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();

	gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].vNormal, 0.0) * MAGNITUDE;
    EmitVertex();
    
	EndPrimitive();
}

void main() {
    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal
}  