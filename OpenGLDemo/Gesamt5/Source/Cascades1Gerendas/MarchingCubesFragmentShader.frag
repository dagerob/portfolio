#version 330 core
out vec4 FragColor;

in vec3 TexCoordsG;
uniform sampler3D density;

void main()
{
     float density = vec4(texture(density, TexCoordsG)).r;
     FragColor = vec4(TexCoordsG.x,TexCoordsG.y,TexCoordsG.z,0);
}           