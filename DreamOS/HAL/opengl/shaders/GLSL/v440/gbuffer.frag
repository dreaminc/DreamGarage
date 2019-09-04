// gbuffer.frag
// shadertype=glsl

// The G-Buffer stage for the use of SSAO / deferred rendering effects

in Data {
	vec4 color;
	vec2 uvCoord;
	vec4 vertWorldSpace;
	vec3 vWorldNormal;
} DataIn;

layout (location = 0) out vec4 out_vec4Positon;
layout (location = 1) out vec4 out_vec4Normal;
layout (location = 2) out vec4 out_vec4AlbedoSpecular;

uniform bool u_hasTextureColor;
layout (binding = 0) uniform sampler2D u_textureColor;

void main(void) {  
	// Store the fragment position vector in the first gbuffer texture
    out_vec4Positon = DataIn.vertWorldSpace;

	// also store the per-fragment normals into the gbuffer
    out_vec4Normal = vec4(normalize(DataIn.vWorldNormal), 0.0f);

    // Diffuse per-fragment color
	if(u_hasTextureColor == true) {
		out_vec4AlbedoSpecular.rgb = texture(u_textureColor, DataIn.uvCoord).rgb;
	}
	else {
		out_vec4AlbedoSpecular.rgb = DataIn.color.rgb;
	}

    // store specular intensity in gAlbedoSpec's alpha component
    out_vec4AlbedoSpecular.a = material.m_shine;
}