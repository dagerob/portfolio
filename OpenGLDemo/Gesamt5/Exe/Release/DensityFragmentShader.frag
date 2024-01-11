#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
uniform float zHeight;
uniform float cameraHeight;
uniform float screenWidth, screenHeight;
uniform float bufferHeight;

vec2 pillars[3] = vec2[3](vec2(0.2,0.6), vec2(0.7,0.8), vec2(0.6,0.3));

// A single iteration of Bob Jenkins' One-At-A-Time hashing algorithm.
uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}

// Compound versions of the hashing algorithm I whipped together.
uint hash( uvec2 v ) { return hash( v.x ^ hash(v.y)                         ); }
uint hash( uvec3 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z)             ); }
uint hash( uvec4 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z) ^ hash(v.w) ); }

// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float floatConstruct( uint m ) {
    const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
    const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32

    m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
    m |= ieeeOne;                          // Add fractional part to 1.0

    float  f = uintBitsToFloat( m );       // Range [1:2]
    return f - 1.0;                        // Range [0:1]
}

// Pseudo-random value in half-open range [0:1].
float random( float x ) { return floatConstruct(hash(floatBitsToUint(x))); }
float random( vec2  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec3  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec4  v ) { return floatConstruct(hash(floatBitsToUint(v))); }

float noise(vec3 val)
{
    float len = val.x;
    int first = int(len / 1);
    int second = int(len / 2);
    int third = int(len / 4);
    int fourth = int(len / 8);

    float xNoise = random(first) + 0.5 * random(second) + 0.25 * random(third) + 0.125 * random(fourth);

    len = val.y;
    first = int(len / 1);
    second = int(len / 2);
    third = int(len / 4);
    fourth = int(len / 8);

    float yNoise = random(first + 1) + 0.5 * random(second + 1) + 0.25 * random(third + 1) + 0.125 * random(fourth + 1);

    len = val.z;
    first = int(len / 1);
    second = int(len / 2);
    third = int(len / 4);
    fourth = int(len / 8);

    float zNoise = random(first +2) + 0.5 * random(second + 2) + 0.25 * random(third + 2) + 0.125 * random(fourth + 2);

    return(xNoise + yNoise + zNoise) /3;
}

void main()
{                
    float thisVal = 0;
    for(int i = 0; i < 3; i++){
        thisVal += 0.05/length(TexCoords - pillars[i].xy) - 0.05;
    }
    vec2 vec = vec2(cos(13 * zHeight/bufferHeight + cameraHeight), sin(13 * zHeight/bufferHeight + cameraHeight));
    thisVal += dot(vec, TexCoords - vec2(0.5));
    thisVal += 0.1 * random(length(TexCoords - vec2(0.5)));
    thisVal = thisVal > 1 ? 1 : thisVal;
    thisVal -= pow(length(TexCoords - vec2(0.5)),1.5);

    FragColor = vec4(thisVal ,0,0,1);
}
