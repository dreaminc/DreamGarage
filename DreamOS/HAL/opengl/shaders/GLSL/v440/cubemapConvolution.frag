// cubemapConvolution.frag
// shadertype=glsl

in Data {
	vec4 color;
	vec3 position;
	vec4 vertWorldSpace;
	vec4 vertViewSpace;
} DataIn;

layout (binding = 0) uniform samplerCube u_textureCubeMap;
uniform bool u_hasTextureCubemap;

layout (location = 0) out vec4 out_vec4Color;

const float PI = 3.14159265359;

void main(void) {  	
	// The world vector acts as the normal of a tangent surface
    // from the origin, aligned to WorldPos. Given this normal, calculate all
    // incoming radiance of the environment. The result of this radiance
    // is the radiance of light coming from -Normal direction, which is what
    // we use in the PBR shader to sample irradiance.

    //vec3 vNormal = normalize(DataIn.vertWorldSpace.xyz);
	vec3 vNormal = normalize(DataIn.position);

    vec3 irradiance = vec3(0.0f);   
    
    // Tangent space calculation from origin point
    vec3 vUp = vec3(0.0f, 1.0f, 0.0f);
    vec3 vRight = cross(vUp, vNormal);
    vUp = cross(vNormal, vRight);
       
    float sampleDelta = 0.025f;
    float nrSamples = 0.0f;

    for(float phi = 0.0f; phi < 2.0f * PI; phi += sampleDelta) {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta) {
            
			// spherical to cartesian (in tangent space)
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));

            // tangent space to world
            vec3 sampleVec = tangentSample.x * vRight + tangentSample.y * vUp + tangentSample.z * vNormal; 

            irradiance += texture(u_textureCubeMap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }

    irradiance = PI * irradiance * (1.0f / float(nrSamples));
    
    out_vec4Color = vec4(irradiance, 1.0);
}