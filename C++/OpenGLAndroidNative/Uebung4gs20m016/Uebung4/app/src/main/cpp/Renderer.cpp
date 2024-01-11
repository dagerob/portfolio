//
// Created by dange on 01/12/2021.
//

#include <android/log.h>
#include <math.h>
#include <GLES3/gl32.h>
#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <time.h>
#include <EGL/egl.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include <GLES2/gl2.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/closest_point.hpp>
#include <string>
#include "OBJ_Loader.h"
#include "MyOBJLoader.h"

#include <GLES2/gl2ext.h>
#include <chrono>

// some useful macros for debugging
#define LOG_TAG "RENDERER"
#define  DEBUG 1
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#if DEBUG
#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#else
#define ALOGV(...)
#endif
// have to be declared in order to know the length of the buffer used to read the OBJ file
#define MAX_VERTICES 500000
// to incorporate the code that should optimize the draw method
#define OPTIMIZED 1
// to activate the time measuring
#define MEASURE_TIME 1

static AAssetManager* g_AssetManager = nullptr;

// where the information will be stored (position, normal and uvs) => 8 floats
float vertices[8*MAX_VERTICES];
unsigned int indices[8*MAX_VERTICES];

glm::mat4 model;    // the model matrix containing the pose of the object relative to world space
glm::mat4 view;     // the camera matrix
glm::mat4 projection;   // the projection matrix for the camera
glm::vec3 lightPos(2,6,2);  // position of the only light
glm::vec3 lightColor(1,1,1);

// openGL variables
GLuint VBO;
GLuint program;
GLuint VAO;
GLuint EBO;
unsigned int textureId;

float screenHeight = 0.0f;
float screenWidth = 0.0f;

// the amount of vertices we need to draw, have to keep track because its based on size ob object loaded form OBJ file
int verticesToDraw = 0;

// method to load the OBJ as a string to be processed from the assests folder
std::string LoadFileContent(AAssetManager* assetManager, const char* path)
{
    AAsset* file = AAssetManager_open(assetManager, path, AASSET_MODE_BUFFER);

    // Get the file length
    size_t fileLength = AAsset_getLength(file);

    // Allocate memory to read your file
    char* fileContent = new char[fileLength+1];

    // Read your file
    AAsset_read(file, fileContent, fileLength);
    // For safety you can add a 0 terminating character at the end of your file ...
    fileContent[fileLength] = '\0';

    std::string ret(fileContent);
    delete[] fileContent;
    return ret;
}

// method to load the pkm texture file from the assets folder into a char buffer
char* LoadTexture(AAssetManager* assetManager, const char* path, size_t& fileLength){
    AAsset* file = AAssetManager_open(assetManager, path, AASSET_MODE_BUFFER);

    // Get the file length
    fileLength = AAsset_getLength(file);

    // Allocate memory to read your file
    char* buffer = new char[fileLength+1];

    // Read your file
    AAsset_read(file, buffer, fileLength);
    AAsset_close(file);
    return buffer;
}

// method used to log GL errors
bool checkGlError(const char* funcName) {
    GLint err = glGetError();
    if (err != GL_NO_ERROR) {
        ALOGE("GL error after %s(): 0x%08x\n", funcName, err);
        return true;
    }
    return false;
}

// method used to compile a shader
GLuint createShader(GLenum shaderType, const char* src) {
    GLuint shader = glCreateShader(shaderType);
    if (!shader) {
        checkGlError("glCreateShader");
        return 0;
    }
    glShaderSource(shader, 1, &src, NULL);

    GLint compiled = GL_FALSE;
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLogLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen > 0) {
            GLchar* infoLog = (GLchar*)malloc(infoLogLen);
            if (infoLog) {
                glGetShaderInfoLog(shader, infoLogLen, NULL, infoLog);
                ALOGE("Could not compile %s shader:\n%s\n",
                      shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment",
                      infoLog);
                free(infoLog);
            }
        }
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

// method to compile a shader program composed of vertex + fragment shader
GLuint createProgram(const char* vtxSrc, const char* fragSrc) {
    GLuint vtxShader = 0;
    GLuint fragShader = 0;
    GLuint program = 0;
    GLint linked = GL_FALSE;

    vtxShader = createShader(GL_VERTEX_SHADER, vtxSrc);
    if (!vtxShader)
        goto exit;

    fragShader = createShader(GL_FRAGMENT_SHADER, fragSrc);
    if (!fragShader)
        goto exit;

    program = glCreateProgram();
    if (!program) {
        checkGlError("glCreateProgram");
        goto exit;
    }
    glAttachShader(program, vtxShader);
    glAttachShader(program, fragShader);

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        ALOGE("Could not link program");
        GLint infoLogLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen) {
            GLchar* infoLog = (GLchar*)malloc(infoLogLen);
            if (infoLog) {
                glGetProgramInfoLog(program, infoLogLen, NULL, infoLog);
                ALOGE("Could not link program:\n%s\n", infoLog);
                free(infoLog);
            }
        }
        glDeleteProgram(program);
        program = 0;
    }

    exit:
    glDeleteShader(vtxShader);
    glDeleteShader(fragShader);
    return program;
}

// method to load the read data form the OBJ file into the vertices array
void createVertices(const lder::Loader& loader) {
    int totalIndices = loader.indices.size();
    int curVertIndex = 0;
    for (int i = 0; i < totalIndices; i += 3) {
        vertices[curVertIndex] = loader.positions[loader.indices[i] - 1].X;
        vertices[curVertIndex + 1] = loader.positions[loader.indices[i] - 1].Y;
        vertices[curVertIndex + 2] = loader.positions[loader.indices[i] - 1].Z;
        vertices[curVertIndex + 3] = loader.normals[loader.indices[i + 2] - 1].X;
        vertices[curVertIndex + 4] = loader.normals[loader.indices[i + 2] - 1].Y;
        vertices[curVertIndex + 5] = loader.normals[loader.indices[i + 2] - 1].Z;
        vertices[curVertIndex + 6] = loader.uvs[loader.indices[i + 1] - 1].X;
        vertices[curVertIndex + 7] = loader.uvs[loader.indices[i + 1] - 1].Y;
        curVertIndex += 8;
    }
}

void createVerticesWithIndices(const lder::Loader& loader){
    int totalIndices = loader.indices.size();
    int thisIndex = 0;
    for(int i = 0; i < totalIndices; i+=3){
        int thisVertexIndex = loader.indices[i]-1;
        int indexToSave = thisVertexIndex * 8;
        int thisNormalIndex = loader.indices[i + 2]-1;
        int thisUVIndex = loader.indices[i + 1]-1;
        // get the positions
        vertices[indexToSave] = loader.positions[thisVertexIndex].X;
        vertices[indexToSave + 1] = loader.positions[thisVertexIndex].Y;
        vertices[indexToSave + 2] = loader.positions[thisVertexIndex].Z;
        // get the normals
        vertices[indexToSave + 3] = loader.normals[thisNormalIndex].X;
        vertices[indexToSave + 4] = loader.normals[thisNormalIndex].Y;
        vertices[indexToSave + 5] = loader.normals[thisNormalIndex].Z;
        // get the uvs
        vertices[indexToSave + 6] = loader.uvs[thisUVIndex].X;
        vertices[indexToSave + 7] = loader.uvs[thisUVIndex].Y;
        // store the vertex index to the indices array
        indices[thisIndex] = loader.indices[i]-1;
        thisIndex++;
    }
    //ALOGV("%f / %f / %f -- %f / %f / %f -- %f / %f", vertices[0], vertices[1], vertices[2], vertices[3], vertices[4], vertices[5], vertices[6], vertices[7]);
}

// method used for decoding the ETC1 texture
static uint16_t getShortBE(char* data) {
    return data[0] << 8 | data[1];
}

// method to load the ETC1 texture and create a glTexture based on that
unsigned int loadEtc1Texture(char* data) {
    // Check for file magic PKM
    assert(data[0] == 'P');
    assert(data[1] == 'K');
    assert(data[2] == 'M');
    assert(data[3] == ' ');
    // Check for version (must be 10)
    assert(data[4] == '1');
    assert(data[5] == '0');

    uint16_t extendedWidth = getShortBE(data + 0x8);
    uint16_t extendedHeight = getShortBE(data + 0xA);

    uint32_t compressedDataSize = (extendedWidth / 4) * (extendedHeight / 4) * 8;
    char* compressedData = data + 0x10;

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_ETC1_RGB8_OES, extendedWidth, extendedHeight, 0, compressedDataSize, compressedData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture;
}

void initialize(){

    // make the texture
    size_t textureLength;
    char* data = LoadTexture(g_AssetManager,"Texture.pkm",textureLength);
    textureId = loadEtc1Texture(data);
    delete[] data;

#if MEASURE_TIME
    // measuring load time start
    auto t1 = std::chrono::high_resolution_clock::now();
#endif
    // load the OBJ file
    lder::Loader loader;
    // first just get a string with all the contents
    std::string modelString = LoadFileContent(g_AssetManager,"Model.obj");
    // then make a stringstream from it and send it to the loader object to be processed
    std::stringstream modelStream(modelString);
    loader.LoadFile(&modelStream);

    // lastly reorganize the data as to be more efficient
#if OPTIMIZED
    createVerticesWithIndices(loader);
#else
    createVertices(loader);
#endif
    // the amount of points we need to draw is given by the amount of indices found,
    // but because each index group has one for the normal and uv, divide by 3
    verticesToDraw = loader.indices.size() / 3;
#if MEASURE_TIME
    // stop measuring load time
    auto t2 = std::chrono::high_resolution_clock::now();

    // print load time
    auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "%i",ms_int);
#endif

    // load the shaders from the assets
    std::string vertexShaderString = LoadFileContent(g_AssetManager,"vertexShader.vert");
    std::string fragmentShaderString = LoadFileContent(g_AssetManager,"fragmentShader.frag");
    // compile the shaders
    program = createProgram(vertexShaderString.c_str(),fragmentShaderString.c_str());

    // create and bind the VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    // create the buffer with data
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

#if OPTIMIZED
    // if trying the optimized version load the indices too
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
#endif
    // bind it to the vertex attribute pointer in position 0 (layout=0 in shaders)
    // for positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // for normals (layout=1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // for uvs (layout=2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glEnable(GL_DEPTH_TEST);

}
// variables needed for Mobile Input
float thisAngle = 0.0f;
float startingAngle = 0.0f;
int firstX1 = -1;
float firstDistance = -1;
glm::vec3 cameraPos(0.0f, 10.0f, 10.0f);
glm::vec3 firstCameraPos(0.0f, 10.0f, 10.0f);
glm::vec3 centerPos(0.0f, 0.0f, 0.0f);
glm::vec3 lookDirection(0.0f, -0.70711f, -0.70711f);

void draw(int x1, int y1, int x2, int y2){
    // measuring draw time start
#if MEASURE_TIME
    auto t1 = std::chrono::high_resolution_clock::now();
#endif

    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    model = glm::mat4(1.0f);
    // just rotate the object on its y axis
    if(x1 >= 0 && x2 < 0){
        if(firstX1 < 0) {
            startingAngle = thisAngle;
            firstX1 = x1;
        }
        thisAngle = startingAngle + (x1 - firstX1)/8.0f;
    }else{
        firstX1 = -1;
    }
    model = glm::rotate(model, glm::radians(thisAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::mat4(1.0f);

    // we have to zoom in and out
    if(x1 >=0 && x2 >= 0){
        float separationSquared = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
        if(firstDistance < 0){
            firstCameraPos = cameraPos;
            firstDistance = separationSquared;
        }
        cameraPos = firstCameraPos + ((separationSquared-firstDistance)/100000)*lookDirection;
    }else{
        firstDistance = -1;
    }
    view = glm::lookAt(cameraPos,
                       centerPos,
                       glm::vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(45.0f), screenWidth / screenHeight, 0.1f, 100.0f);

    glUseProgram(program);

    // set all needed values on the shaders
    int modelLoc = glGetUniformLocation(program, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    int viewLoc = glGetUniformLocation(program, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    int projLoc = glGetUniformLocation(program, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    int lightPosLoc = glGetUniformLocation(program, "lightPos");
    glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
    int lightColorLoc = glGetUniformLocation(program, "lightColor");
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    // set the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glBindVertexArray(VAO);

#if OPTIMIZED
    glDrawElements(GL_TRIANGLES, verticesToDraw, GL_UNSIGNED_INT, 0);
#else
    glDrawArrays(GL_TRIANGLES, 0, verticesToDraw);
#endif

#if MEASURE_TIME
    // stop measuring draw time
    auto t2 = std::chrono::high_resolution_clock::now();

    // print load time
    auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "%i",ms_int);
#endif
}

//-------------------------------------------------------------------------------------------------------------

extern "C" JNIEXPORT void JNICALL
Java_com_example_uebung3_GLLib_init(JNIEnv* env, jclass obj, jobject assetManager) {
    g_AssetManager = AAssetManager_fromJava(env, assetManager);
    initialize();

}

extern "C" JNIEXPORT void JNICALL
Java_com_example_uebung3_GLLib_resize(JNIEnv* env, jclass obj, jint width, jint height) {
    screenWidth = width;
    screenHeight = height;
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_uebung3_GLLib_step(JNIEnv* env, jclass obj, jint x1, jint y1, jint x2, jint y2) {
    draw(x1, y1, x2, y2);
}