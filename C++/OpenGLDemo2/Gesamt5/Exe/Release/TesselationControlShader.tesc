#version 410 core

// define the number of CPs in the output patch
layout (vertices = 3) out;

uniform vec3 gEyeWorldPos;
uniform float tl;

in VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec3 Tangent;
	vec2 TexCoords;
} tesc_in [];

out VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec3 Tangent;
	vec2 TexCoords;
} tesc_out [];

void main()
{
    // Set the control points of the output patch
	tesc_out[gl_InvocationID].FragPos = tesc_in[gl_InvocationID].FragPos;
	tesc_out[gl_InvocationID].Normal = tesc_in[gl_InvocationID].Normal;
	tesc_out[gl_InvocationID].TexCoords = tesc_in[gl_InvocationID].TexCoords;
	tesc_out[gl_InvocationID].Tangent = tesc_in[gl_InvocationID].Tangent;

	// set the Tesselation Levels
	gl_TessLevelOuter[0] = tl;
	gl_TessLevelOuter[1] = tl;
	gl_TessLevelOuter[2] = tl;
	gl_TessLevelInner[0] = tl;
}