// blinnPhongTexTBNBump.vert
// shadertype=glsl

// This is the standard Dream shader model
// and will include light, material, and texturing capabilities

// TODO: Move to a uniform block
layout (location = 0) in vec4 inV_vec4Position;
layout (location = 1) in vec4 inV_vec4Color;
layout (location = 2) in vec4 inV_vec4Normal;
layout (location = 3) in vec2 inV_vec2UVCoord;
layout (location = 4) in vec4 inV_vec4Tangent;
layout (location = 5) in vec4 inV_vec4Bitangent;

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
	vec3 vertTBNSpace;
	float riverAnimationDisplacement;
} DataOut;

uniform vec4 u_vec4Eye;
uniform mat4 u_mat4Model;
uniform mat4 u_mat4View;
uniform mat4 u_mat4ModelView;
uniform mat4 u_mat4ViewProjection;
uniform mat4 u_mat4Normal;

uniform bool u_fRiverAnimation;
uniform float u_time;

// TODO: Move to CPU side
mat4 g_mat4ModelView = u_mat4View * u_mat4Model;
mat4 g_mat4InvTransposeModelView = transpose(inverse(g_mat4ModelView));
//mat4 g_mat4InvTransposeModel = transpose(inverse(u_mat4Model));

vec4 RiverAnimation(vec4 position, vec4 vertWorldSpace) {
	float arg = vertWorldSpace.x + vertWorldSpace.z + u_time;		
	
	float scale = 0.1f;
	float displacement = scale * sin(arg);
	///*
	displacement += scale * sin(2.2f * arg + 5.52f);
	displacement += scale * sin(2.9f * arg + 0.93f);
	displacement += scale * sin(4.6f * arg + 8.94f);
	displacement *= 0.25f;
	displacement -= 0.1f;
	DataOut.riverAnimationDisplacement = displacement;
	//*/

	return vec4(position.x, position.y + displacement, position.z, position.w);
}

void main(void) {	
	vec4 vertWorldSpace = u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);
	vec4 vertViewSpace = u_mat4View * u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);

	// BTN Matrix
	DataOut.TangentBitangentNormalMatrix = CalculateTBNMatrix(g_mat4InvTransposeModelView, inV_vec4Tangent, inV_vec4Normal);
	DataOut.directionEye = DataOut.TangentBitangentNormalMatrix * (-normalize(vertViewSpace.xyz));
	vec4 vec4ModelNormal = g_mat4InvTransposeModelView * normalize(vec4(inV_vec4Normal.xyz, 0.0f));
	
	for(int i = 0; i < numLights; i++) {
		ProcessLightVertex(lights[i], u_mat4View, vertViewSpace, vertWorldSpace, DataOut.directionLight[i], DataOut.distanceLight[i]);

		// Apply TBN matrix 
		DataOut.directionLight[i] = normalize(DataOut.TangentBitangentNormalMatrix * DataOut.directionLight[i]);
	}

	DataOut.vertTBNSpace = DataOut.TangentBitangentNormalMatrix * vertViewSpace.xyz;
	DataOut.vertWorldSpace = vertWorldSpace;
	DataOut.vertViewSpace = vertViewSpace;
	DataOut.normal = vec4ModelNormal;
	DataOut.uvCoord = inV_vec2UVCoord;

	// Vert Color
	DataOut.color = inV_vec4Color;

	// Projected Vert Position
	vec4 position = u_mat4ViewProjection * vertWorldSpace;

	if (u_fRiverAnimation) {
		gl_Position = RiverAnimation(position, vertWorldSpace);
	} 
	else {
		gl_Position = position;
	}
}