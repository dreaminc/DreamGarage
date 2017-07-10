// minimalTexture.vert
// shadertype=glsl

// This is a minimal texture shader that simply displays
// a vertex color and position (no lighting)

#version 440 core

in Data {
	vec4 color;
	vec2 uvCoord;
	vec4 vertClipSpace;
} DataIn;

uniform bool u_hasTextureColor;
uniform sampler2D u_textureColor;

uniform mat4 u_mat4ClippingProjection;
uniform bool u_clippingEnabled;

struct Material {
	float m_shine;
	float m_bump;
	float m_ambient;
	float reserved3;
    vec4 m_colorAmbient;
    vec4 m_colorDiffuse;
    vec4 m_colorSpecular;
};

layout(std140) uniform ub_material {
    Material material;
};

layout (location = 0) out vec4 out_vec4Color;

//vec4 g_ambient = vec4(0.05f);
vec4 g_ambient = vec4(0.0f);

void main(void) {  
	vec4 color = DataIn.color;

	// TODO: Might be able to do this in vertex and interpolate to 
	// speed this up at cost of accuracy
	if(u_clippingEnabled == true) {
		float xDiff = 1.0f - abs(DataIn.vertClipSpace.x);
		float yDiff = 1.0f - abs(DataIn.vertClipSpace.y);
		float zDiff = 1.0f - abs(DataIn.vertClipSpace.z);

		if(xDiff < 0.0f || yDiff < 0.0f || zDiff < 0.0f) {
			discard;
		}
		else {
			float knee = 0.1f;
			float minDistance = min(min(xDiff, yDiff), zDiff);
			float ratio = (knee - minDistance) / knee;

			if(ratio > 0.0f) {
				color.a = (1.0f - ratio);
			}
		}
	}

	if(u_hasTextureColor == true) {
		color = texture(u_textureColor, DataIn.uvCoord);
	}

	out_vec4Color = material.m_colorDiffuse * color + g_ambient;

	//out_vec4Color = material.m_colorDiffuse;
	//out_vec4Color = DataIn.color * textureColor + g_ambient;
}