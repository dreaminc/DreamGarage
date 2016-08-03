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
uniform mat4 u_mat4ModelView;
uniform mat4 u_mat4ViewProjection;
uniform mat4 u_mat4Normal;

uniform mat4 u_mat4DepthVP;

uniform bool u_boolIsBillboard;
uniform bool u_boolShouldScale;

uniform vec4 u_vec4Center;
uniform vec4 u_vec4CamOrigin;

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

// billboarding matrices
vec3 dir = -normalize(u_vec4CamOrigin.xyz + u_vec4Center.xyz);

vec3 rightV = vec3( u_mat4View[0][0],
					u_mat4View[1][0],
					u_mat4View[2][0]);

vec3 upV = vec3(	u_mat4View[0][1],
					u_mat4View[1][1],
					u_mat4View[2][1]);


vec3 y = vec3(0.0f, 1.0f, 0.0f);

vec3 rightV2 = normalize(cross(y, dir));
vec3 upV2 = normalize(cross(rightV2, dir));

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

		float scale = 1.0f;
		if (u_boolShouldScale) {
			// May be a hack, but it makes sense and looks right
			//TODO expose these?
			float scalingMagnitude = 0.1f;
			float farScale = 3.0f;

			scale = max(1.0f, length(u_vec4CamOrigin.xyz + u_vec4Center.xyz)*scalingMagnitude);

			if (scale > farScale) 
				scale = 0.0f;
		}

		vec3 worldX = inV_vec4Position.x * normalize(rightV2) * scale;
		vec3 worldY = inV_vec4Position.z * normalize(upV2) * scale;
		vec3 worldZ = vec3(0.0f, 0.0f, inV_vec4Position.y);

		vec3 position_world = u_vec4Center.xyz + worldX + worldY + worldZ;

		gl_Position = u_mat4ViewProjection * vec4(position_world.xyz, 1.0f);
	}
}