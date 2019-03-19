// minimalTexture_clipping.vert
// shadertype=glsl

// This is a minimal texture shader that simply displays
// a vertex color and position (no lighting)

in Data {
	vec4 color;
	vec2 uvCoord;
	vec4 vertWorldSpace;
	float vertDepth;
} DataIn;

uniform bool	u_hasTextureColor;
layout (binding = 0) uniform sampler2D u_textureColor;

layout (location = 0) out vec4 out_vec4Color;

uniform vec4 u_vec4ClippingPlane;
uniform float u_clippingOffset;

//vec4 g_ambient = vec4(0.05f);
vec4 g_ambient = vec4(0.0f);

void main(void) {  
	// Clip fragments on our side of the plane
	float fragmentClipPosition = dot(DataIn.vertWorldSpace.xyz, normalize(u_vec4ClippingPlane.xyz)) + u_vec4ClippingPlane.w;
    //if (fragmentClipPosition < u_clippingOffset) {
	if (fragmentClipPosition < 0.0f) {
		discard;
    }
	
	vec4 textureColor = vec4(1.0f);
	
	if (u_hasTextureColor == true) {
		textureColor = texture(u_textureColor, DataIn.uvCoord);
	}
	else {
		textureColor = DataIn.color;
	}
	
	vec4 shaderColor = 1.5f * material.m_colorDiffuse * textureColor + g_ambient;

	out_vec4Color = MixWithFog(FOG_TYPE_LINEAR, shaderColor, DataIn.vertDepth);
}