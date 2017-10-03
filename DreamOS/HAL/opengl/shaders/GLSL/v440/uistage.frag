// minimalTexture.vert
// shadertype=glsl

// This is a minimal texture shader that simply displays
// a vertex color and position (no lighting)

#version 440 core

in Data {
	vec4 color;
	vec2 uvCoord;
	vec4 ptMid;
} DataIn;

uniform bool u_hasTextureColor;
uniform sampler2D u_textureColor;

uniform bool u_clippingEnabled;
uniform vec4 u_ptOrigin;
uniform vec4 u_vOrigin;

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

float g_knee = 0.1f;
float g_cosThreshold = 0.56f;
float g_fadeRate = 6.0f;

void main(void) {  
	vec4 color = DataIn.color;

	// TODO: Might be able to do this in vertex and interpolate to 
	// speed this up at cost of accuracy

	if(u_hasTextureColor == true) {
		color = texture(u_textureColor, DataIn.uvCoord);
	}

	if(u_clippingEnabled == true) {
	
		vec3 vOrigin = normalize(vec3(u_vOrigin.x, 0.0f, u_vOrigin.z));
		vec3 vOrigintoQuadVertex = normalize(vec3(DataIn.ptMid.x - u_ptOrigin.x, 0.0f, DataIn.ptMid.z - u_ptOrigin.z));
		float angle = dot(vOrigin, vOrigintoQuadVertex);

		float minDistance = angle - g_cosThreshold;

		if (minDistance < 0.0f) {
			discard;
		}

		float ratio = (g_knee - minDistance) / g_knee;
		if (ratio > 0.0f) {
			//scale function to fit domain of [0,1]
			float x = ratio-0.5f;

			//scale y to range of [0,1]
			float y = (tanh(g_fadeRate*x)+1.0f)/2.0f;

			color.a = color.a * (1.0 - y);

			//potentially normalize color
			//vec3 white = vec3(1.0f, 1.0f, 1.0f);
			//color.rgb = color.rgb + (white - (white * (1.0 - y)));
		}
	}

	out_vec4Color = material.m_colorDiffuse * color + g_ambient;

	//out_vec4Color = material.m_colorDiffuse;
	//out_vec4Color = DataIn.color * textureColor + g_ambient;
}