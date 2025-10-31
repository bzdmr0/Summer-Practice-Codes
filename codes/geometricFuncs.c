#include <GLFW/glfw3.h>
#include <GLES2/gl2.h>
#include <stdio.h>
#include <stdlib.h>

void init();
void draw();

// Static global variables
static GLFWwindow *window;
static GLuint basicProgram, lengthProgram, distanceProgram, normalizeProgram;
static GLuint faceforwardProgram, reflectProgram, refractProgram;
static unsigned int rectangleVBO;

// Uniform locations
static GLint lenVecLoc, distVec1Loc, distVec2Loc, normalizeVecLoc;
static GLint faceforwardNLoc, faceforwardILoc, faceforwardNrefLoc;
static GLint reflectILoc, reflectNLoc;
static GLint refractILoc, refractNLoc, refractEtaLoc;

// Test vectors
static float vec_Len[3] = {2.0f, 4.0f, 4.0f}; // a vector which has length 6.0
static float vec1_Distance[2] = {1.0f, 2.0f};
static float vec2_Distance[2] = {4.0f, 6.0f};
static float vec_Normalize[3] = {3.0f, 4.0f, 5.0f}; // a vector to normalize

// Faceforward test vectors
static float vec_N[3] = {0.0f, 1.0f, 0.0f};        // normal vector (pointing up)
static float vec_I[3] = {1.0f, -1.0f, 0.0f};       // incident vector
static float vec_Nref[3] = {0.0f, -1.0f, 0.0f};    // reference vector (pointing down)

// Reflect test vectors
static float reflect_I[3] = {1.0f, -1.0f, 0.0f};   // incident vector (diagonal down-right)
static float reflect_N[3] = {0.0f, 1.0f, 0.0f};    // normal vector (pointing up)

// Refract test vectors
static float refract_I[3] = {0.707f, -0.707f, 0.0f}; // normalized incident vector (45 degrees)
static float refract_N[3] = {0.0f, 1.0f, 0.0f};      // normal vector (pointing up)
static float refract_eta = 0.75f;                     // refraction index ratio (air to glass)

static int g_width = 1280, g_height = 720;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(g_width, g_height, "GLSL Geometric Functions", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);

    init();

    while (!glfwWindowShouldClose(window)) {
        draw();
    }

    glDeleteBuffers(1, &rectangleVBO);
    glDeleteProgram(basicProgram);
    glDeleteProgram(lengthProgram);
    glDeleteProgram(distanceProgram);
    glDeleteProgram(normalizeProgram);
    glDeleteProgram(faceforwardProgram);
    glDeleteProgram(reflectProgram);
    glDeleteProgram(refractProgram);

    glfwTerminate();
    printf("Program terminated.\n");
    return 0;
}

void init() {

    // Embedded shader strings
    const char *BasicVS = "#version 100\n"
                          "attribute vec3 aPos;\n"
                          "void main() {\n"
                          "    gl_Position = vec4(aPos, 1.0);\n"
                          "}";

    const char *BasicFS = "#version 100\n"
                          "precision mediump float;\n"
                          "void main() {\n"
                          "    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
                          "}";

    const char *LengthFS = "#version 100\n"
                           "precision mediump float;\n"
                           "uniform vec3 uVec;\n"
                           "void main() {\n"
                           "    float length = length(uVec);\n"
                           "    vec4 color;\n"
                           "    if(abs(length - 6.0) < 0.01){\n"
                           "        color = vec4(0.0, 1.0, 0.0, 1.0);\n"
                           "    } else{\n"
                           "        color = vec4(1.0, 0.0, 0.0, 1.0);\n"
                           "    }\n"
                           "    gl_FragColor = color;\n"
                           "}";

    const char *DistanceFS = "#version 100\n"
                             "precision mediump float;\n"
                             "uniform vec2 uVec1;\n"
                             "uniform vec2 uVec2;\n"
                             "void main() {\n"
                             "    float distance = distance(uVec1, uVec2);\n"
                             "    vec4 color;\n"
                             "    if(distance == 5.0){\n"
                             "        color = vec4(0.0, 1.0, 0.0, 1.0);\n"
                             "    } else{\n"
                             "        color = vec4(1.0, 0.0, 0.0, 1.0);\n"
                             "    }\n"
                             "    gl_FragColor = color;\n"
                             "}";

    const char *NormalizeFS = "#version 100\n"
                              "precision mediump float;\n"
                              "uniform vec3 uVec;\n"
                              "void main() {\n"
                              "    vec3 normalizedVec = normalize(uVec);\n"
                              "    float len = length(normalizedVec);\n"
                              "    vec4 color;\n"
                              "    if(abs(len-1.0) < 0.01){\n"
                              "        color = vec4(0.0, 1.0, 0.0, 1.0);\n"
                              "    } else{\n"
                              "        color = vec4(1.0, 0.0, 0.0, 1.0);\n"
                              "    }\n"
                              "    gl_FragColor = color;\n"
                              "}";

    const char *FaceforwardFS = "#version 100\n"
                                "precision mediump float;\n"
                                "uniform vec3 uN;\n"
                                "uniform vec3 uI;\n"
                                "uniform vec3 uNref;\n"
                                "void main() {\n"
                                "    vec3 result = faceforward(uN, uI, uNref);\n"
                                "    float dotProduct = dot(uNref, uI);\n"
                                "    vec3 expected;\n"
                                "    if (dotProduct < 0.0) {\n"
                                "        expected = uN;\n"
                                "    } else {\n"
                                "        expected = -uN;\n"
                                "    }\n"
                                "    vec3 diff = result - expected;\n"
                                "    float error = length(diff);\n"
                                "    vec4 color;\n"
                                "    if (error < 0.01) {\n"
                                "        color = vec4(0.0, 1.0, 0.0, 1.0);\n"
                                "    } else {\n"
                                "        color = vec4(1.0, 0.0, 0.0, 1.0);\n"
                                "    }\n"
                                "    gl_FragColor = color;\n"
                                "}";

    const char *ReflectFS = "#version 100\n"
                            "precision mediump float;\n"
                            "uniform vec3 uI;\n"
                            "uniform vec3 uN;\n"
                            "void main() {\n"
                            "    vec3 result = reflect(uI, uN);\n"
                            "    float dotNI = dot(uN, uI);\n"
                            "    vec3 expected = uI - 2.0 * dotNI * uN;\n"
                            "    vec3 diff = result - expected;\n"
                            "    float error = length(diff);\n"
                            "    vec4 color;\n"
                            "    if (error == 0.0) {\n"
                            "        color = vec4(0.0, 1.0, 0.0, 1.0);\n"
                            "    } else {\n"
                            "        color = vec4(1.0, 0.0, 0.0, 1.0);\n"
                            "    }\n"
                            "    gl_FragColor = color;\n"
                            "}";

    const char *RefractFS = "#version 100\n"
                            "precision mediump float;\n"
                            "uniform vec3 uI;\n"
                            "uniform vec3 uN;\n"
                            "uniform float uEta;\n"
                            "void main() {\n"
                            "    vec3 result = refract(uI, uN, uEta);\n"
                            "    float dotNI = dot(uN, uI);\n"
                            "    float k = 1.0 - uEta * uEta * (1.0 - dotNI * dotNI);\n"
                            "    vec3 expected;\n"
                            "    if (k < 0.0) {\n"
                            "        expected = vec3(0.0, 0.0, 0.0);\n"
                            "    } else {\n"
                            "        expected = uEta * uI - (uEta * dotNI + sqrt(k)) * uN;\n"
                            "    }\n"
                            "    vec3 diff = result - expected;\n"
                            "    float error = length(diff);\n"
                            "    vec4 color;\n"
                            "    if (error < 0.01) {\n"
                            "        color = vec4(0.0, 1.0, 0.0, 1.0);\n"
                            "    } else {\n"
                            "        color = vec4(1.0, 0.0, 0.0, 1.0);\n"
                            "    }\n"
                            "    gl_FragColor = color;\n"
                            "}";

    // Create vertex shader (shared by all programs)
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &BasicVS, NULL);
    glCompileShader(vertexShader);

    // Create fragment shaders
    GLuint basicFShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(basicFShader, 1, &BasicFS, NULL);
    glCompileShader(basicFShader);

    GLuint lengthFShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(lengthFShader, 1, &LengthFS, NULL);
    glCompileShader(lengthFShader);

    GLuint distanceFShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(distanceFShader, 1, &DistanceFS, NULL);
    glCompileShader(distanceFShader);

    GLuint normalizeFShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(normalizeFShader, 1, &NormalizeFS, NULL);
    glCompileShader(normalizeFShader);

    GLuint faceforwardFShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(faceforwardFShader, 1, &FaceforwardFS, NULL);
    glCompileShader(faceforwardFShader);

    GLuint reflectFShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(reflectFShader, 1, &ReflectFS, NULL);
    glCompileShader(reflectFShader);

    GLuint refractFShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(refractFShader, 1, &RefractFS, NULL);
    glCompileShader(refractFShader);

    // Create shader programs
    basicProgram = glCreateProgram();
    glAttachShader(basicProgram, vertexShader);
    glAttachShader(basicProgram, basicFShader);
    glLinkProgram(basicProgram);

    lengthProgram = glCreateProgram();
    glAttachShader(lengthProgram, vertexShader);
    glAttachShader(lengthProgram, lengthFShader);
    glLinkProgram(lengthProgram);

    distanceProgram = glCreateProgram();
    glAttachShader(distanceProgram, vertexShader);
    glAttachShader(distanceProgram, distanceFShader);
    glLinkProgram(distanceProgram);

    normalizeProgram = glCreateProgram();
    glAttachShader(normalizeProgram, vertexShader);
    glAttachShader(normalizeProgram, normalizeFShader);
    glLinkProgram(normalizeProgram);

    faceforwardProgram = glCreateProgram();
    glAttachShader(faceforwardProgram, vertexShader);
    glAttachShader(faceforwardProgram, faceforwardFShader);
    glLinkProgram(faceforwardProgram);

    reflectProgram = glCreateProgram();
    glAttachShader(reflectProgram, vertexShader);
    glAttachShader(reflectProgram, reflectFShader);
    glLinkProgram(reflectProgram);

    refractProgram = glCreateProgram();
    glAttachShader(refractProgram, vertexShader);
    glAttachShader(refractProgram, refractFShader);
    glLinkProgram(refractProgram);

    // Clean up shaders
    glDeleteShader(vertexShader);
    glDeleteShader(basicFShader);
    glDeleteShader(lengthFShader);
    glDeleteShader(distanceFShader);
    glDeleteShader(normalizeFShader);
    glDeleteShader(faceforwardFShader);
    glDeleteShader(reflectFShader);
    glDeleteShader(refractFShader);

    // Rectangle vertices
    float rectangleVertices[] = {
        -1.0f, -1.0f, 0.0f, // bottom left
        1.0f, -1.0f, 0.0f, // bottom right
        -1.0f,  1.0f, 0.0f, // top left
        1.0f, -1.0f, 0.0f, // bottom right
        1.0f,  1.0f, 0.0f, // top right
        -1.0f,  1.0f, 0.0f  // top left
    };

    // Create buffers
    glGenBuffers(1, &rectangleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, rectangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Get uniform locations
    lenVecLoc = glGetUniformLocation(lengthProgram, "uVec");
    distVec1Loc = glGetUniformLocation(distanceProgram, "uVec1");
    distVec2Loc = glGetUniformLocation(distanceProgram, "uVec2");
    normalizeVecLoc = glGetUniformLocation(normalizeProgram, "uVec");

    // Faceforward uniform locations
    faceforwardNLoc = glGetUniformLocation(faceforwardProgram, "uN");
    faceforwardILoc = glGetUniformLocation(faceforwardProgram, "uI");
    faceforwardNrefLoc = glGetUniformLocation(faceforwardProgram, "uNref");

    // Reflect uniform locations
    reflectILoc = glGetUniformLocation(reflectProgram, "uI");
    reflectNLoc = glGetUniformLocation(reflectProgram, "uN");

    // Refract uniform locations
    refractILoc = glGetUniformLocation(refractProgram, "uI");
    refractNLoc = glGetUniformLocation(refractProgram, "uN");
    refractEtaLoc = glGetUniformLocation(refractProgram, "uEta");
}

void draw() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Length test
    glViewport(0,(g_height/2)+5 , (g_width/3)-10, (g_height/2)-5); // [0,0]
    glUseProgram(lengthProgram);
    glUniform3fv(lenVecLoc, 1, vec_Len);
    glDrawArrays(GL_TRIANGLES,0,6);

    // Distance test
    glViewport((g_width/3)+5,(g_height/2)+5 , (g_width/3)-10, (g_height/2)-5); // [0,1]
    glUseProgram(distanceProgram);
    glUniform2fv(distVec1Loc, 1, vec1_Distance);
    glUniform2fv(distVec2Loc, 1, vec2_Distance);
    glDrawArrays(GL_TRIANGLES,0,6);

    // Normalize test
    glViewport((2*g_width/3)+10,(g_height/2)+5 , (g_width/3)-10, (g_height/2)-5); // [0,2]
    glUseProgram(normalizeProgram);
    glUniform3fv(normalizeVecLoc, 1, vec_Normalize);
    glDrawArrays(GL_TRIANGLES,0,6);

    // Faceforward test
    glViewport(0,0 , (g_width/3)-10, (g_height/2)-5); // [1,0]
    glUseProgram(faceforwardProgram);
    glUniform3fv(faceforwardNLoc, 1, vec_N);
    glUniform3fv(faceforwardILoc, 1, vec_I);
    glUniform3fv(faceforwardNrefLoc, 1, vec_Nref);
    glDrawArrays(GL_TRIANGLES,0,6);

    // Reflect test
    glViewport((g_width/3)+5,0 , (g_width/3)-10, (g_height/2)-5); // [1,1]
    glUseProgram(reflectProgram);
    glUniform3fv(reflectILoc, 1, reflect_I);
    glUniform3fv(reflectNLoc, 1, reflect_N);
    glDrawArrays(GL_TRIANGLES,0,6);

    // Refract test
    glViewport((2*g_width/3)+10,0 , (g_width/3)-10, (g_height/2)-5); // [1,2]
    glUseProgram(refractProgram);
    glUniform3fv(refractILoc, 1, refract_I);
    glUniform3fv(refractNLoc, 1, refract_N);
    glUniform1f(refractEtaLoc, refract_eta);
    glDrawArrays(GL_TRIANGLES,0,6);

    glfwSwapBuffers(window);
    glfwPollEvents();
}
