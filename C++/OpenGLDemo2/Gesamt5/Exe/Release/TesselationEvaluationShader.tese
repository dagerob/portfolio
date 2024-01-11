#version 410 core

layout(triangles, fractional_odd_spacing, ccw) in;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec3 Tangent;
	vec2 TexCoords;
} tese_in [];

out TSE_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	mat3 TBN;
} tese_out;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

void main()
{
    // Interpolate the attributes of the output vertex using the barycentric coordinates
	tese_out.TexCoords = interpolate2D(tese_in[0].TexCoords, tese_in[1].TexCoords, tese_in[2].TexCoords); 
	tese_out.Normal = interpolate3D(tese_in[0].Normal, tese_in[1].Normal, tese_in[2].Normal);
	tese_out.Normal = normalize(tese_out.Normal);
	vec3 aTangent = interpolate3D(tese_in[0].Tangent, tese_in[1].Tangent, tese_in[2].Tangent);
	aTangent = normalize(tese_out.Normal);
	tese_out.FragPos = interpolate3D(tese_in[0].FragPos, tese_in[1].FragPos, tese_in[2].FragPos);

	vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
	vec3 N = normalize(transpose(inverse(mat3(model))) * tese_out.Normal);
	vec3 B = cross(T, N);
	tese_out.TBN = mat3(T, B, N);

	gl_Position = projection * view * vec4(tese_out.FragPos, 1.0);
}