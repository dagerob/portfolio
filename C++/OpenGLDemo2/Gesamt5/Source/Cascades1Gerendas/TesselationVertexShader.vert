#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

out VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec3 Tangent;
	vec2 TexCoords;
} vs_out;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
	vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
	vs_out.Tangent = transpose(inverse(mat3(model))) * aTangent;
	vs_out.TexCoords = aTexCoord;
	//gl_Position = vec4(vs_out.FragPos, 1.0);
}