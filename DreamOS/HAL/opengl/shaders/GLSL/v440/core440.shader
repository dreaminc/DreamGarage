// core440.shader
// shadertype=glsl

// A shared GLSL library for core GL 4.4.0 

// This should be the first shader given this establishes the version directive below

// This may also be used for general purpose utilities and definitions

#version 440 core

mat3 CalculateTBNMatrix(in mat4 mat3ModelView, in vec4 vTangent, in vec4 vNormal) {
	
	// BTN Matrix
	mat3 TBNTransformMatrix = mat3(mat3ModelView);

	// Either the tangent of certain geometry is wrong, or this is not needed
	vec3 vModelTangent = normalize(TBNTransformMatrix * vTangent.xyz);
	vec3 vModelNormal = normalize(TBNTransformMatrix * vNormal.xyz);

	// Re-orthogonalize T with respect to N with Gram-Schmidt process
	vModelTangent = normalize(vModelTangent - dot(vModelTangent, vModelNormal) * vModelNormal);

	// Then calc bitangent using normal and tangent
	vec3 vModelBitangent = normalize(cross(vModelNormal, vModelTangent));

	mat3 mat3TBNMatrix =
		transpose(mat3(
			vModelTangent,
			vModelBitangent,
			vModelNormal)
		);

	return mat3TBNMatrix;
}

