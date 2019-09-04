// brdfCommon.shader
// shadertype=glsl

// A shared GLSL library for vertex shader stage BRDF shading functionality

// BRDF

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

vec4 ambientIBL(samplerCube irradianceMap, mat4 mat4Model, vec4 eyePosition, vec4 vertWorldSpace, vec4 vNormal, bool fGrayscale) {
	
	vec4 vWorldViewDirection = normalize(vertWorldSpace - eyePosition);
	vec4 vWorldNormal = normalize(mat4Model * vec4(vNormal.xyz, 0.0f));
	
	// Irradiance look up

	vec3 F0 = vec3(0.04f);		// F0 - for plastic is 0.04
	//vec3 albedo = material.m_colorDiffuse.xyz;
	vec3 albedo = vec3(1.0f);
	float ao = 1.0f;
	float roughness = 0.95f;

	vec3 kS = fresnelSchlickRoughness(max(dot(vWorldNormal.xyz, vWorldViewDirection.xyz), 0.0f), F0, roughness);
	vec3 kD = vec3(1.0f) - kS;
	vec3 irradiance = texture(irradianceMap, vWorldNormal.xyz).rgb;
	vec3 diffuse = irradiance * albedo;
	vec3 ambient = (kD * diffuse) * ao;

	// This will return grayscale
	if (fGrayscale == true) {
		ambient = vec3(((ambient.x + ambient.y + ambient.z) / 3.0f));
	}

	return vec4(ambient, 1.0f);
}

vec4 reflectionIBL(samplerCube environmentMap, mat4 mat4Model, vec4 eyePosition, vec4 vertWorldSpace, vec4 vNormal) {

	vec4 vWorldViewDirection = normalize(vertWorldSpace - eyePosition);
	vec4 vWorldNormal = normalize(mat4Model * vec4(vNormal.xyz, 0.0f));

	vec3 vReflection = reflect(vWorldViewDirection.xyz, vWorldNormal.xyz);
	
	vec4 colorReflect = vec4(texture(environmentMap, vReflection).rgb, 1.0);

	return colorReflect;
}

vec4 refractionIBL(samplerCube environmentMap, mat4 mat4Model, vec4 eyePosition, vec4 vertWorldSpace, vec4 vNormal) {

	float refractRatio = 1.00 / 1.52;

	vec4 vWorldViewDirection = normalize(vertWorldSpace - eyePosition);
	vec4 vWorldNormal = normalize(mat4Model * vec4(vNormal.xyz, 0.0f));

	vec3 vRefraction = refract(vWorldViewDirection.xyz, vWorldNormal.xyz, refractRatio);

	vec4 colorRefract = vec4(texture(environmentMap, vRefraction).rgb, 1.0);

	return colorRefract;
}