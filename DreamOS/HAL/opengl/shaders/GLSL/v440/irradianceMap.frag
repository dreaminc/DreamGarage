// irradianceMap.frag
// shadertype=glsl

// Irradiance image based lighing shader

in vec3 inF_vec3Color;

in Data {
    vec4 normal;
    vec3 directionEye;
	vec3 directionLight[MAX_TOTAL_LIGHTS];
	float distanceLight[MAX_TOTAL_LIGHTS];
	vec4 color;
	vec4 vertWorldSpace;
	vec4 vertViewSpace;
	vec4 normalOrig;
} DataIn;

uniform vec4 u_vec4Eye;
uniform mat4 u_mat4Model;
uniform mat4 u_mat4View;

layout (binding = 0) uniform samplerCube u_textureCubeMap;
uniform bool u_hasTextureCubemap;

layout (location = 0) out vec4 out_vec4Color;

mat4 g_mat4ModelView = u_mat4View * u_mat4Model;
mat4 g_mat4InvTransposeModelView = transpose(inverse(g_mat4ModelView));

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}   

void main(void) {  
	
	vec4 vec4LightValue = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	float diffuseValue = 0.0f;
	float specularValue = 0.0f;
	
	vec3 normal = normalize(DataIn.normal.xyz);
	vec3 directionEye = -normalize(DataIn.vertViewSpace.xyz);

	for(int i = 0; i < numLights; i++) {
		vec3 directionLight = normalize(DataIn.directionLight[i]);

		if(dot(normal, directionLight) > 0.0f) {
			CalculateFragmentLightValue(lights[i].m_power, material.m_shine, normal, directionLight, directionEye, DataIn.distanceLight[i], diffuseValue, specularValue);
			//CalculateFragmentLightValue(lights[i].m_power, 60.0f, normal, directionLight, directionEye, DataIn.distanceLight[i], diffuseValue, specularValue);
			
			vec4LightValue += diffuseValue * lights[i].m_colorDiffuse * material.m_colorDiffuse;
			vec4LightValue += specularValue * lights[i].m_colorSpecular * material.m_colorSpecular;
		}
	}
	vec4LightValue[3] = 1.0f;

	vec4 vWorldViewDirection = normalize(DataIn.vertWorldSpace - u_vec4Eye);
	vec4 vWorldNormal = normalize(u_mat4Model * vec4(DataIn.normalOrig.xyz, 0.0f));
	vec3 vReflection = reflect(vWorldViewDirection.xyz, vWorldNormal.xyz);

	//vec4 ambientColor = material.m_ambient * material.m_colorAmbient;
	vec4 ambientColor = vec4(0.0f);

	// Environment Map
	if(u_hasTextureCubemap == true) {
		
		// Irradiance look up
		
		vec3 F0 = vec3(0.04f);		// F0 - for plastic is 0.04
		//vec3 albedo = material.m_colorDiffuse.xyz;
		vec3 albedo = vec3(1.0f);
		float ao = 1.0f;
		float roughness = 0.25f;

		vec3 kS = fresnelSchlickRoughness(max(dot(vWorldNormal.xyz, vWorldViewDirection.xyz), 0.0f), F0, roughness);
		vec3 kD = vec3(1.0f) - kS;
		vec3 irradiance = texture(u_textureCubeMap, vWorldNormal.xyz).rgb;
		vec3 diffuse = irradiance * albedo;
		vec3 ambient = (kD * diffuse) * ao; 

		// Try to average it
		//ambient = vec3(1.0f) * ((ambient.x + ambient.y + ambient.z) / 3.0f);

		ambientColor = vec4(ambient, 1.0f);

		// Reflection - but this should not actually be used (TODO: move to standard tho)
		// vec4 colorReflect = vec4(texture(u_textureCubeMap, vReflection).rgb, 1.0);	
		////vec4LightValue *= colorReflect;
		// vec4LightValue = colorReflect + vec4LightValue * 0.2f;
	}
	
	//out_vec4Color = vec4LightValue;
	//out_vec4Color = (vec4LightValue * DataIn.color) + ambientColor;

	out_vec4Color = max((vec4LightValue * DataIn.color), ambientColor);
}