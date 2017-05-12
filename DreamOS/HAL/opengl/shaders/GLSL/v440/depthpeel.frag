// depthpeel.vert
// shadertype=glsl

// The depth peeling shader discards fragments with depth
// less than the front buffer if set 

#version 440 core

in Data {
	vec4 color;
} DataIn;

layout (location = 0) out vec4 out_vec4Color;

// Depth texture 
uniform sampler2DRect u_textureDepth;

uniform bool u_fDiscard;

void main(void) {  
	// Compare the current fragment depth with the depth in the depth texture
	// if it is less, discard the current fragment
	if(u_fDiscard) {
		// Read the depth value from the depth texture
		float frontDepth = texture(u_textureDepth, gl_FragCoord.xy).r;

		if(gl_FragCoord.z <= frontDepth) {
			discard;
		}
	}
	
	// NOTE: This is just doing minimal stuff at the moment
	out_vec4Color = DataIn.color;
}