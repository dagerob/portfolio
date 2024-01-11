#version 330 core
out vec4 FragColor;

in VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
	mat3 TBN;
} fs_in;

uniform sampler2D albedoTexture;
uniform sampler2D bumpTexture;
uniform sampler2D normalTexture;
uniform sampler2D roughTexture;
uniform sampler2D shadowMap;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 cameraFront;
uniform float bumpiness;
uniform bool useDisplacement;
uniform int steps;
uniform float duv;
int shadowSize = 1024;

float color = 0;

vec2 getMorphedUVs(vec2 oldUVs){
	int N = steps;
	vec2 finalUV = oldUVs;
	float dH = 1.0f / N;
	vec2 dUV = -cameraFront.xy * duv; // 0.001
	int firstHitIterator = 0;
	for(int i = 0; i < N; i++){
		vec2 thisUVs = oldUVs + i * dUV;
		float readH = texture(bumpTexture,thisUVs).r;
		float rayH = i * dH;
		if(readH > rayH){
			finalUV = thisUVs;
			firstHitIterator = i;
		}
	}
	vec2 nextUVs = finalUV + dUV;
	float dh1 = texture(bumpTexture,finalUV).r - dH * firstHitIterator; // the difference between the last found point and the ray on that points
	float dh2 = texture(bumpTexture, nextUVs).r - clamp(dH * (firstHitIterator + 1),0.0f, 1.0f); // the difference in hs int the next step (already above geometry)

	float weight = dh1 / (dh1 - dh2);
	finalUV = nextUVs * weight + finalUV * (1.0 - weight);

	return finalUV;
}

float blur(vec2 fragCoord, float c)
{
    float Pi = 6.28318530718; // Pi*2
    
    // GAUSSIAN BLUR SETTINGS {{{
    float Directions = 16.0; // BLUR DIRECTIONS (Default 16.0 - More is better but slower)
    float Quality = 5.0; // BLUR QUALITY (Default 4.0 - More is better but slower)
    float Size = 8.0; // BLUR SIZE (Radius)
    // GAUSSIAN BLUR SETTINGS }}}
   
    vec2 Radius = vec2(Size/shadowSize);
    
    // Pixel colour
    float Color = texture(shadowMap, fragCoord).r;
    
    // Blur calculations
    for( float d=0.0; d<Pi; d+=Pi/Directions)
    {
		for(float i=1.0/Quality; i<=1.0; i+=1.0/Quality)
        {
			float thisVal = texture( shadowMap, fragCoord+vec2(cos(d),sin(d))*Radius*i).r;	
			Color += exp(c*thisVal);
        }
    }
    
    // Output to screen
    Color /= Quality * Directions - 15.0;
    return Color;
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	//float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.007);
	float bias = 0.007;
	//float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	float c = 40;
	float blurredExpZ = blur(projCoords.xy, c);
	float shadow = clamp(exp(-c*currentDepth)*blurredExpZ, 0, 1);
	return shadow;
}



void main()
{
	vec2 illusionUVs = useDisplacement ? getMorphedUVs(fs_in.TexCoords) : fs_in.TexCoords;

	// sample the albedo color
    vec4 baseColor = texture(albedoTexture,illusionUVs);

	// calculate normal based on rgb normalmap
	vec3 norm = texture(normalTexture, illusionUVs).xyz;
	norm = norm * 2.0 - 1.0;
	norm = normalize(fs_in.TBN * norm);
	norm = mix(norm,fs_in.Normal,min(max(bumpiness,0.0),1.0));

	// calculate normal based on bump map
//	float delta = 1.0f/1024;
//	float bumpFactor = 1;
//	vec2 uvp1 = fs_in.TexCoords + vec2(delta,0);
//	vec2 uvp2 = fs_in.TexCoords + vec2(-delta,0);
//	vec2 uvp3 = fs_in.TexCoords + vec2(0, delta);
//	vec2 uvp4 = fs_in.TexCoords + vec2(0, -delta);
//	float x = texture(bumpTexture,uvp1).r - texture(bumpTexture,uvp2).r;
//	float y = texture(bumpTexture,uvp3).r - texture(bumpTexture,uvp4).r;
//	float z = 1.0f;
//	vec3 norm = vec3(x * bumpFactor, y * bumpFactor, z);
//	norm = normalize(fs_in.TBN * norm);

	// calculate light direction
	vec3 lightDir = normalize(lightPos - fs_in.FragPos); 

	// calculate diffuse lighting
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * baseColor.rgb;

	//calculate specular lighting
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	vec3 reflectDir = reflect(-lightDir, norm); 
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 4);
	vec3 specular = (1 - texture(roughTexture,illusionUVs).r) * 2 * spec * vec3(1,1,1); 

	// calculate shadows
	float shadow = ShadowCalculation(fs_in.FragPosLightSpace,norm, lightDir);

	// get total lighting value

	FragColor = (vec4(diffuse, 1.0) + vec4(specular, 1.0)) * shadow + baseColor * 0.2;
	//FragColor = vec4(color);
} 