// minimal.vert
// shadertype=glsl

#version 440 core

#define MAX_TOTAL_LIGHTS 10

// TODO: Move to a uniform block
layout (location = 0) in vec4 inV_vec4Position;
layout (location = 1) in vec4 inV_vec4Color;
layout (location = 2) in vec4 inV_vec4Normal;
layout (location = 3) in vec2 inV_vec2UVCoord;
layout (location = 4) in vec4 inV_vec4Tangent;
layout (location = 5) in vec4 inV_vec4Bitangent;

//out vec3 inF_vec3Color;

out Data {
	vec4 normal;
	vec3 directionEye;
	vec3 directionLight[MAX_TOTAL_LIGHTS];
	float distanceLight[MAX_TOTAL_LIGHTS];
	vec4 color;
	vec2 uvCoord;
	vec4 vertWorldSpace;
	vec4 vertViewSpace;
	mat3 TangentBitangentNormalMatrix;
} DataOut;

uniform vec4 u_vec4Eye;
uniform mat4 u_mat4Model;
uniform mat4 u_mat4View;
uniform mat4 u_mat4ModelView;
uniform mat4 u_mat4ViewProjection;
uniform mat4 u_mat4Normal;

// Light Structure
struct Light {
	int m_type;
	float m_power;
	float m_shine;
	float m_reserved;

	vec4 m_ptOrigin;
	vec4 m_colorDiffuse;
	vec4 m_colorSpecular;
	vec4 m_vectorDirection; 
};

layout(std140) uniform ub_LightArray {
	Light lights[MAX_TOTAL_LIGHTS];
	int numLights;	
};

// TODO: Move to CPU side
mat4 g_mat4ModelView = u_mat4View * u_mat4Model;
mat4 g_mat4InvTransposeModelView = transpose(inverse(g_mat4ModelView));
//mat4 g_mat4InvTransposeModel = transpose(inverse(u_mat4Model));

void main(void) {	
	vec4 vertWorldSpace = u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);
	vec4 vertViewSpace = u_mat4View * u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);

	DataOut.directionEye = -normalize(vertViewSpace.xyz);
	vec4 vec4ModelNormal = g_mat4InvTransposeModelView * normalize(vec4(inV_vec4Normal.xyz, 0.0f));
	
	for(int i = 0; i < numLights; i++) {
		vec3 ptLightViewSpace = vec3(u_mat4View * lights[i].m_ptOrigin);
		DataOut.directionLight[i] = normalize(ptLightViewSpace.xyz - vertViewSpace.xyz);
		DataOut.distanceLight[i] = length(lights[i].m_ptOrigin.xyz - vertWorldSpace.xyz);
	}

	DataOut.vertWorldSpace = vertWorldSpace;
	DataOut.vertViewSpace = vertViewSpace;
	DataOut.normal = vec4ModelNormal;
	DataOut.uvCoord = inV_vec2UVCoord;

	// BTN Matrix
	// TODO: All vectors to tangent space in vert shader?
	// TODO: Calc this CPU side?  Understand tradeoffs 
	//mat3 TBNTransformMatrix = mat3(u_mat4Model);
	//mat3 TBNTransformMatrix = mat3(g_mat4InvTransposeModelView);
	mat3 TBNTransformMatrix = mat3(g_mat4ModelView);
	//mat3 TBNTransformMatrix = mat3(u_mat4View);

	vec3 ModelTangent = normalize(TBNTransformMatrix * inV_vec4Tangent.xyz);
	vec3 ModelBitangent = normalize(TBNTransformMatrix * inV_vec4Bitangent.xyz);
	vec3 ModelNormal = normalize(TBNTransformMatrix * inV_vec4Normal.xyz);

	//DataOut.TangentBitangentNormalMatrix = transpose(mat3(ModelTangent, ModelBitangent, ModelNormal));
	DataOut.TangentBitangentNormalMatrix = mat3(ModelTangent, ModelBitangent, ModelNormal);

	// Vert Color
	DataOut.color = inV_vec4Color;

	// Projected Vert Position
	gl_Position = u_mat4ViewProjection * vertWorldSpace;
}