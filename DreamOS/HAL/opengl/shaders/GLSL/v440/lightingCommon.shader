// lightingCommon.shader
// shadertype=glsl

// A shared GLSL library for vertex shader stage lighting functionality

#define MAX_TOTAL_LIGHTS 10

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

// Input layout for light block
layout(std140) uniform ub_Lights {
	Light lights[MAX_TOTAL_LIGHTS];
	int numLights;	
};

void ProcessLightVertex(in Light light, in mat4 mat4View, in vec4 vertViewSpace, in vec4 vertWorldSpace, out vec3 vLightDirection, out float lightDistance) {
	if(light.m_type == 0) {
		// Directional Light
		vLightDirection = normalize(vec3(mat3(mat4View) * (-light.m_vectorDirection.xyz)));
		lightDistance = 0.0f;
	}
	else  {
		// Point Light
		vec3 ptLightViewSpace = vec3(mat4View * vec4(light.m_ptOrigin.xyz, 1.0f));
		
		vLightDirection = normalize(ptLightViewSpace.xyz - vertViewSpace.xyz);
		lightDistance = length(light.m_ptOrigin.xyz - vertWorldSpace.xyz);
	}
}
