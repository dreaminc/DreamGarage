// minimal.vert
// shadertype=glsl

in Data {
	vec4 color;
	vec3 position;
	vec4 vertWorldSpace;
	vec4 vertViewSpace;
} DataIn;

uniform samplerCube u_textureCubeMap;
uniform bool u_hasTextureCubemap;

layout (location = 0) out vec4 out_vec4Color;

void main(void) {  	
	vec4 outColor = vec4(1.0f);

	if(u_hasTextureCubemap == true) {
		//outColor = texture(u_textureCubeMap, normalize(DataIn.position));
		outColor = texture(u_textureCubeMap, DataIn.position);
	}

	// DEBUG
	//outColor = vec4(normalize(DataIn.position), 1.0f);

	out_vec4Color = vec4(outColor.rgb, 1.0f);
}