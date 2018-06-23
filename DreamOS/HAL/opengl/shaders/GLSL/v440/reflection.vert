// reflection.vert
// shadertype=glsl

// This is a reflection shader and will reflect the camera against 
// a reflection matrix provided 

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
	vec4 vertEyeSpace;
	mat3 TangentBitangentNormalMatrix;
	vec3 vertTBNSpace;
	float riverAnimationDisplacement;
} DataOut;

uniform vec4 u_vec4Eye;
uniform mat4 u_mat4Model;
uniform mat4 u_mat4View;
uniform mat4 u_mat4ModelView;
//uniform mat4 u_mat4ViewProjection;
uniform mat4 u_mat4Projection;
uniform mat4 u_mat4Normal;
uniform mat4 u_mat4Reflection;

uniform vec4 u_vec4ReflectionPlane;

// TODO: Move to CPU side
mat4 xzFlipMatrix = mat4(1.0f, 0.0f, 0.0f, 0.0f,
						 0.0f, -1.0f, 0.0f, 0.0f,
						 0.0f, 0.0f, 1.0f, 0.0f,
						 0.0f, 0.0f, 0.0f, 1.0f);

mat4 g_mat4ReflectedView = u_mat4Reflection * u_mat4View;// * xzFlipMatrix;	// This could easily be done on the CPU side
//mat4 g_mat4ReflectedView = u_mat4Reflection * u_mat4View;	// This could easily be done on the CPU side

mat4 g_mat4ModelView = g_mat4ReflectedView * u_mat4Model;
mat4 g_mat4InvTransposeModelView = transpose(inverse(g_mat4ModelView));
mat4 g_mat4ViewProjection = u_mat4Projection * g_mat4ReflectedView;

void main(void) {	
	vec4 vertWorldSpace = u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);
	vec4 vertViewSpace = g_mat4ModelView * vec4(inV_vec4Position.xyz, 1.0f);
	vec4 vertEyeSpace = u_mat4View * u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);

	// BTN Matrix
	DataOut.TangentBitangentNormalMatrix = CalculateTBNMatrix(g_mat4InvTransposeModelView, inV_vec4Tangent, inV_vec4Normal);
	DataOut.directionEye = DataOut.TangentBitangentNormalMatrix * (-normalize(vertViewSpace.xyz));
	vec4 vec4ModelNormal = g_mat4InvTransposeModelView * normalize(vec4(inV_vec4Normal.xyz, 0.0f));
	
	for(int i = 0; i < numLights; i++) {
		ProcessLightVertex(lights[i], g_mat4ReflectedView, vertViewSpace, vertWorldSpace, DataOut.directionLight[i], DataOut.distanceLight[i]);

		// Apply TBN matrix 
		DataOut.directionLight[i] = normalize(DataOut.TangentBitangentNormalMatrix * DataOut.directionLight[i]);
	}

	DataOut.vertTBNSpace = DataOut.TangentBitangentNormalMatrix * vertViewSpace.xyz;
	DataOut.vertWorldSpace = vertWorldSpace;
	DataOut.vertViewSpace = vertViewSpace;
	DataOut.vertEyeSpace = vertEyeSpace;
	DataOut.normal = vec4ModelNormal;
	DataOut.uvCoord = inV_vec2UVCoord;

	// Vert Color
	DataOut.color = inV_vec4Color;

	// Projected Vert Position
	//gl_Position = g_mat4ViewProjection * vertWorldSpace;
	gl_Position = u_mat4Projection * vertViewSpace;
}