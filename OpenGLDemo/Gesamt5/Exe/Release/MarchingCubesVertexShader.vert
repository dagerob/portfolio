#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;
out int varIndex;

uniform sampler3D density;

uniform float bufferlength, bufferwidth, bufferheight;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

float stepX = (1 / bufferwidth);
float stepY = (1 / bufferlength);
float stepZ = (1 / bufferheight);

float threshold = 0.5f;

void main()
{
    //gl_PointSize = 4;
    gl_Position = vec4(aPos, 1.0);

    float thisTexX = aPos.x / bufferwidth;
    float thisTexY = aPos.y / bufferlength;
    float thisTexZ = aPos.z / bufferheight;

    TexCoords = vec3(thisTexX,thisTexY,thisTexZ);

    int b1 = int(texture(density, TexCoords).r < threshold);
    int b2 = int(texture(density, TexCoords + vec3(stepX, 0.0, 0.0)).r < threshold);
    int b3 = int(texture(density, TexCoords + vec3(stepX, 0.0, stepZ)).r < threshold);
    int b4 = int(texture(density, TexCoords + vec3(0.0, 0.0, stepZ)).r < threshold);
    int b5 = int(texture(density, TexCoords + vec3(0.0, stepY, 0.0)).r < threshold);
    int b6 = int(texture(density, TexCoords + vec3(stepX, stepY, 0.0)).r < threshold);
    int b7 = int(texture(density, TexCoords + vec3(stepX, stepY, stepZ)).r < threshold);
    int b8 = int(texture(density, TexCoords + vec3(0.0, stepY, stepZ)).r < threshold);
    varIndex = (b1 << 7) | (b2 << 6) | (b3 << 5) | (b4 << 4) | 
               (b5 << 3) | (b6 << 2) | (b7 << 1) | b8;
}