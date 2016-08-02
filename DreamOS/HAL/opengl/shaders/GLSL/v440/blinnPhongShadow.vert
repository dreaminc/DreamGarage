// minimal.vert
// shadertype=glsl

// This is a minimal shader that simply displays
// a vertex color and position (no lighting, no textures)

#version 440 core

#define MAX_TOTAL_LIGHTS 10

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
	vec4 vertWorldSpace;
	vec4 vertViewSpace;
	
	vec4 vertShadowCoordinate;
	vec3 directionShadowCastingLight;
} DataOut;

uniform vec4 u_vec4Eye;
uniform mat4 u_mat4Model;
uniform mat4 u_mat4View;
uniform mat4 u_mat4Projection;
uniform mat4 u_mat4ModelView;
uniform mat4 u_mat4ViewProjection;
uniform mat4 u_mat4Normal;

uniform mat4 u_mat4DepthVP;

uniform bool u_boolIsBillboard;
uniform vec4 u_vec3Center;

// Light Structure
// TODO: Create a parsing system to create shader GLSL code
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

layout(std140) uniform ub_Lights {
	Light lights[MAX_TOTAL_LIGHTS];
	int numLights;	
};

// TODO: Move to CPU side
mat4 g_mat4ModelView = u_mat4View * u_mat4Model;
mat4 g_mat4InvTransposeModelView = transpose(inverse(g_mat4ModelView));

mat4 biasMat = mat4(0.5, 0.0, 0.0, 0.0,
					0.0, 0.5, 0.0, 0.0,
					0.0, 0.0, 0.5, 0.0,
					0.5, 0.5, 0.5, 1.0);


mat3 g_mat3X90 = mat3(	1.0,0.0,0.0,
						0.0,0.0,1.0,
						0.0,-1.0,0.0);

mat4 g_mat4X90 = mat4(	1.0, 0.0, 0.0, 0.0,
						0.0, 0.0, 1.0, 0.0,
						0.0,-1.0, 0.0, 0.0,
						0.0, 0.0, 0.0, 1.0);	



void main(void) {	
	vec4 vertWorldSpace = u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);
	vec4 vertViewSpace = u_mat4View * u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);
	vec4 vertDepthSpace = biasMat * u_mat4DepthVP * u_mat4Model * vec4(inV_vec4Position.xyz, 1.0f);
	
	vec4 vec4ModelNormal = g_mat4InvTransposeModelView * normalize(vec4(inV_vec4Normal.xyz, 0.0f));
	
	for(int i = 0; i < numLights; i++) {
		
		if(lights[i].m_type == 0) {
			// Directional Light
			DataOut.directionLight[i] = normalize(vec3(mat3(u_mat4View) * (-lights[i].m_vectorDirection.xyz)));
			DataOut.distanceLight[i] = 0.0f;
		}
		else  {
			// Point Light
			vec3 ptLightViewSpace = vec3(u_mat4View * vec4(lights[i].m_ptOrigin.xyz, 1.0f));
			DataOut.directionLight[i] = normalize(ptLightViewSpace.xyz - vertViewSpace.xyz);
			DataOut.distanceLight[i] = length(lights[i].m_ptOrigin.xyz - vertWorldSpace.xyz);
		}
	}

	// TODO: Fix this
	DataOut.directionShadowCastingLight = vec3(0.0f, 1.0f, 0.0f);
	DataOut.vertShadowCoordinate = vertDepthSpace;
	DataOut.directionEye = -normalize(vertViewSpace.xyz);
	DataOut.vertWorldSpace = vertWorldSpace;
	DataOut.vertViewSpace = vertViewSpace;
	DataOut.normal = vec4ModelNormal;

	// Vert Color
	DataOut.color = inV_vec4Color;

	// Projected Vert Position
	if (!u_boolIsBillboard) {
		gl_Position = u_mat4ViewProjection * vertWorldSpace;
	} else {
		
		// remove rotation from modelview matrix
/*		g_mat4ModelView[0][0] = 1;
		g_mat4ModelView[0][1] = 0;
		g_mat4ModelView[0][2] = 0;

		g_mat4ModelView[1][0] = 0;
		g_mat4ModelView[1][1] = 0;
		g_mat4ModelView[1][2] = 1;

		g_mat4ModelView[2][0] = 0;
		g_mat4ModelView[2][1] = -1;
		g_mat4ModelView[2][2] = 0;

		vec3 y = vec3( 0.0, 1.0, 0.0);

		vec3 yProj = (dot(lookV,y)/dot(y,y))*y;
		vec3 yRej = normalize(lookV - yProj);
		vec3 lookBillboard = -yRej;

		vec3 n = vec4ModelNormal.xyz;

		vec3 nProj = (dot(n,y)/dot(y,y))*y;
		vec3 nRej = normalize(n-nProj);

		//float theta = acos(dot(lookV, n));
		float theta = acos((dot(lookBillboard, nRej)));

		mat4 g_mat4YRot = mat4(	cos(theta),	0.0, sin(theta),	0.0,
								0.0,		1.0, 0.0,			0.0,
								-sin(theta), 0.0, cos(theta),	0.0,
								0.0,		0.0, 0.0,			1.0);

		mat4 g_mat4XRot = mat4(1.0, 0.0, 0.0, 0.0,
								0.0, cos(theta), sin(theta), 0.0,
								0.0, -sin(theta), cos(theta), 0.0,
								0.0, 0.0, 0.0, 1.0);

		mat4 g_mat4ZRot = mat4(cos(theta), sin(theta), 0.0, 0.0,
								-sin(theta), cos(theta), 0.0, 0.0,
								0.0, 0.0, 1.0, 0.0,
								0.0, 0.0, 0.0, 1.0);
*/


		mat4 g_mat4View = (u_mat4View);
		vec3 lookV = vec3(	g_mat4View[0][2],
							g_mat4View[1][2],
							g_mat4View[2][2]);

		vec3 rightV = vec3( g_mat4View[0][0],
							g_mat4View[1][0],
							g_mat4View[2][0]);

		vec3 upV = vec3(	g_mat4View[0][1],
							g_mat4View[1][1],
							g_mat4View[2][1]);


		vec3 position_world = u_vec3Center.xyz + inV_vec4Position.xyz;
		vec3 model = vec3(vertWorldSpace.x,vertWorldSpace.y, vertWorldSpace.z);
		vec3 worldX = vec3(inV_vec4Position.x, 0.0f, 0.0f);
		vec3 worldY = vec3(0.0f, inV_vec4Position.z, 0.0f);
		vec3 worldZ = vec3(0.0f, 0.0f, inV_vec4Position.y);
		worldY = (-inV_vec4Position.z) * normalize(upV);
		worldX = (inV_vec4Position.x) * normalize(rightV);
		position_world = u_vec3Center.xyz + worldX + worldY + worldZ;
		vec4 center = vec4(0.0f, 0.0f, 0.0f, 1.0f);

		//gl_Position = u_mat4Projection * u_mat4View * vec4(position_world.xyz, 1.0f);	
		mat4 g_mat4Y90 = mat4(	0.0, 0.0, 1.0, 0.0,
								0.0, 1.0, 0.0, 0.0,
							   -1.0, 0.0, 0.0, 0.0,
								0.0, 0.0, 0.0, 1.0);	

		mat4 g_mat4Z180 = mat4(-1.0, 0.0, 0.0, 0.0,
								0.0,-1.0, 0.0, 0.0,
								0.0, 0.0, 1.0, 0.0, 
								0.0, 0.0, 0.0, 1.0);

		//gl_Position = u_mat4Projection * u_mat4View * u_mat4Model * g_mat4Z180 * inverse(u_mat4Model) * vec4(position_world.xyz, 1.0f);
		gl_Position = u_mat4Projection * u_mat4View * u_mat4Model * vec4(position_world.xyz, 1.0f);

/*


		for (int x = 0; x < 3; x++) {
			for (int z = 0; z < 3; z++) {
				g_mat4ModelView[x][z] = (g_mat4YRot)[x][z];
			}
		}
*/
//		vec3 u_vec3Center = vec3(0.0f, 0.0f, 0.0f);
//		vec3 position_world = u_vec3Center + (rightV * (vertWorldSpace.x - u_vec3Center.x)) + (upV * (vertWorldSpace.y - u_vec3Center.y));
		//gl_Position = u_mat4Projection * g_mat4ModelView * g_mat4X90 * g_mat4Y90 * vec4(inV_vec4Position.xyz, 1.0f);	
//		gl_Position = u_mat4Projection * u_mat4View * u_mat4Model * vec4(position_world.xyz, 1.0f);	
	}
}