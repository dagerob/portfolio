#version 410 core
out vec4 FragColor;

in TSE_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	mat3 TBN;
} fs_in;

uniform sampler2D albedoTexture;
uniform sampler2D normalTexture;
uniform sampler2D roughTexture;
uniform vec3 lightPos;
uniform vec3 viewPos;

float color;

void main()
{
	// sample the albedo color
    vec4 baseColor = texture(albedoTexture,fs_in.TexCoords);

	// calculate normal based on rgb normalmap
	vec3 norm = texture(normalTexture, fs_in.TexCoords).xyz;
	norm = norm * 2.0 - 1.0;
	norm = normalize(fs_in.TBN * norm);
	norm = mix(norm,fs_in.Normal,0.5);

	// calculate light direction
	vec3 lightDir = normalize(lightPos - fs_in.FragPos); 

	// calculate diffuse lighting
	float diff = max(dot(norm, lightDir), 0.0);
	color = norm.z;
	vec3 diffuse = diff * baseColor.rgb;

	//calculate specular lighting
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	vec3 reflectDir = reflect(-lightDir, norm); 
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 4);
	vec3 specular = (1 - texture(roughTexture,fs_in.TexCoords).r) * 2 * spec * vec3(1,1,1); 

	// get total lighting value

	FragColor = (vec4(diffuse, 1.0) + vec4(specular, 1.0)) * 1 + baseColor * 0.2;
	//FragColor = vec4(color);
} 