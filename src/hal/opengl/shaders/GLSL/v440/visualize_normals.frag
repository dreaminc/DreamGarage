// visualize_normals.vert
// shadertype=glsl

// This is a minimal fragment shader for the visualization of normals
// program

layout (location = 0) out vec4 out_vec4Color;

void main(void) {  
	out_vec4Color = vec4(1.0, 1.0, 0.0, 1.0);
}