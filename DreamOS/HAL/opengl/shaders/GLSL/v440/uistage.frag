// minimalTexture.vert
// shadertype=glsl

// This is a minimal texture shader that simply displays
// a vertex color and position (no lighting)

#version 440 core

in Data {
	vec4 color;
	vec2 uvCoord;
	//vec4 vertClipSpace;
	//float angle;
	vec4 ptMid;
} DataIn;

uniform bool u_hasTextureColor;
uniform sampler2D u_textureColor;

//uniform mat4 u_mat4ClippingProjection;
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

void main(void) {  
	vec4 color = DataIn.color;

	// TODO: Might be able to do this in vertex and interpolate to 
	// speed this up at cost of accuracy

	if(u_hasTextureColor == true) {
		color = texture(u_textureColor, DataIn.uvCoord);
	}

	if(u_clippingEnabled == true) {
	
		vec3 dotOrigin = normalize(vec3(u_vOrigin.x, 0.0f, u_vOrigin.z));
		vec3 dotDir = normalize(vec3(DataIn.ptMid.x - u_ptOrigin.x, 0.0f, DataIn.ptMid.z - u_ptOrigin.z));
		vec3 right = normalize(cross(vec3(0.0f, 1.0f, 0.0f),dotDir));
		float angle = dot(dotOrigin, dotDir);

		float knee = 0.05f;
		float minDistance = angle - 0.7f;
		if (minDistance < 0.0f) {
			discard;
		}

		float ratio = (knee - minDistance) / knee;
		if (ratio > 0.0f) {
			color.a = color.a * (1.0f - ratio);
		}
	}

	out_vec4Color = material.m_colorDiffuse * color + g_ambient;

	//out_vec4Color = material.m_colorDiffuse;
	//out_vec4Color = DataIn.color * textureColor + g_ambient;
}