// blinnPhongTexTBNBump.vert
// shadertype=glsl

// This shader implements Blinn Phong lighting with color and bump textures
// using a BTN matrix

// Depends: lightingCommon.shader, materialCommon.shader

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
	vec4 tangent;
	vec3 directionLight[MAX_TOTAL_LIGHTS];
	float distanceLight[MAX_TOTAL_LIGHTS];
	vec4 color;
	vec2 uvCoord;
	vec4 vertViewSpace;
	//mat3 TangentBitangentNormalMatrix;
} DataOut;

uniform mat4 u_mat4Model;
uniform mat4 u_mat4View;
uniform mat4 u_mat4ViewProjection;

uniform vec4 u_vec4Eye;

// TODO: Move to CPU side
mat4 g_mat4ModelView = u_mat4View * u_mat4Model;
mat4 g_mat4InvTransposeModelView = transpose(inverse(g_mat4ModelView));

void main(void) {	
	vec4 vertWorldSpace = u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);
	vec4 vertViewSpace = u_mat4View * u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);

	// TBN Matrix
	//DataOut.TangentBitangentNormalMatrix = CalculateTBNMatrix(u_mat4Model, inV_vec4Tangent, inV_vec4Normal);	
	
	for(int i = 0; i < numLights; i++) {
		ProcessLightVertex(lights[i], u_mat4View, vertViewSpace, vertWorldSpace, DataOut.directionLight[i], DataOut.distanceLight[i]);

		// Apply TBN matrix 
		//DataOut.directionLight[i] = normalize(DataOut.TangentBitangentNormalMatrix * DataOut.directionLight[i]);		
	}

	DataOut.vertViewSpace = vertViewSpace;
	DataOut.normal = inV_vec4Normal;
	DataOut.tangent = inV_vec4Tangent;
	DataOut.uvCoord = inV_vec2UVCoord;

	// Vert Color
	DataOut.color = inV_vec4Color;

	// Projected Vert Position
	gl_Position = u_mat4ViewProjection * vertWorldSpace;
}