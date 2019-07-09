// screenquad.vert
// shadertype=glsl

// This is a minimal shader that simply displays a texture to screen quad

#version 440 core

in Data {
	vec4 color;
	vec2 uvCoord;
} DataIn;

//uniform sampler2D u_textureColor;
uniform sampler2DMS u_textureColor;

layout (location = 0) out vec4 out_vec4Color;

uniform vec4 u_vec4BackgroundColor;

float lookupOffsets[4] = float[]( -1.5, -0.5, 0.5, 1.5 );

void main(void) {  
	// Look up texture by coord
	//vec4 color = texture(u_textureColor, DataIn.uvCoord * 1.0f);

	vec4 colorAccumulator = vec4(0.0f);

	 for (int i = 0 ; i < 4 ; i++) {
        for (int j = 0 ; j < 4 ; j++) {
			vec2 tc = gl_FragCoord.xy;
			//tc.x = TexCoord.x + lookupOffsets[j] / textureSize(u_textureColor, 0).x;
            //tc.y = TexCoord.y + lookupOffsets[i] / textureSize(u_textureColor, 0).y;

			tc.x += lookupOffsets[j];
            tc.y += lookupOffsets[i];

            colorAccumulator += texelFetch(u_textureColor, ivec2(tc), i);
		}
	}

	vec4 color = colorAccumulator / 16;
	
	//float param = color.x;

	// Add to background color
	//out_vec4Color = color + u_vec4BackgroundColor * DataIn.color.a;
	//out_vec4Color = vec4(param, param, param, 1.0f);
	out_vec4Color = color;
}