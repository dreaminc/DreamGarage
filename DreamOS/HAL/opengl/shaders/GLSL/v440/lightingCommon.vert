// lightingCommon.vert
// shadertype=glsl

// A shared GLSL library for vertex shader stage lighting functionality

#version 440 core

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