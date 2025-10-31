#include <GLFW/glfw3.h>
#include <GLES2/gl2.h>
#include <stdio.h>
#include <stdlib.h>

void init();
void draw();

// Static global variables
static GLFWwindow *window;
static GLuint basicProgram, lessThanProgram, lessThanEqualProgram, greaterThanProgram;
static GLuint greaterThanEqualProgram, equalProgram, notEqualProgram, anyProgram;
static GLuint allProgram, notProgram, degreesProgram;
static unsigned int rectangleVBO;

// Uniform locations
static GLint lessThanVec1Loc, lessThanVec2Loc;
static GLint lessThanEqualVec1Loc, lessThanEqualVec2Loc;
static GLint greaterThanVec1Loc, greaterThanVec2Loc;
static GLint greaterThanEqualVec1Loc, greaterThanEqualVec2Loc;
static GLint equalVec1Loc, equalVec2Loc;
static GLint notEqualVec1Loc, notEqualVec2Loc;
static GLint anyVecLoc, allVecLoc, notVecLoc;
static GLint degreesRadLoc;

// Test vectors for comparisons
static float vec1_Compare[3] = {1.0f, 2.0f, 3.0f};
static float vec2_Compare[3] = {2.0f, 2.0f, 1.0f};
static float vec1_Equal[3] = {1.0f, 2.0f, 3.0f};
static float vec2_Equal[3] = {1.0f, 2.0f, 3.0f};
static float radians_Test = 3.14159f / 2.0f; // 90 degrees in radians

static int g_width = 1280, g_height = 720;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(g_width, g_height, "GLSL Vector Relational Functions", NULL, NULL);
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
    glDeleteProgram(lessThanProgram);
    glDeleteProgram(lessThanEqualProgram);
    glDeleteProgram(greaterThanProgram);
    glDeleteProgram(greaterThanEqualProgram);
    glDeleteProgram(equalProgram);
    glDeleteProgram(notEqualProgram);
    glDeleteProgram(anyProgram);
    glDeleteProgram(allProgram);
    glDeleteProgram(notProgram);
    glDeleteProgram(degreesProgram);

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

    const char *LessThanFS = "#version 100\n"
                             "precision mediump float;\n"
                             "uniform vec3 uVec1;\n"
                             "uniform vec3 uVec2;\n"
                             "void main() {\n"
                             "    bvec3 result = lessThan(uVec1, uVec2);\n"
                             "    bvec3 expected = bvec3(true, false, false);\n"
                             "    vec4 color;\n"
                             "    if(result == expected){\n"
                             "        color = vec4(0.0, 1.0, 0.0, 1.0);\n"
                             "    } else{\n"
                             "        color = vec4(1.0, 0.0, 0.0, 1.0);\n"
                             "    }\n"
                             "    gl_FragColor = color;\n"
                             "}";

    const char *LessThanEqualFS = "#version 100\n"
                                  "precision mediump float;\n"
                                  "uniform vec3 uVec1;\n"
                                  "uniform vec3 uVec2;\n"
                                  "void main() {\n"
                                  "    bvec3 result = lessThanEqual(uVec1, uVec2);\n"
                                  "    bvec3 expected = bvec3(true, true, false);\n"
                                  "    vec4 color;\n"
                                  "    if(result == expected){\n"
                                  "        color = vec4(0.0, 1.0, 0.0, 1.0);\n"
                                  "    } else{\n"
                                  "        color = vec4(1.0, 0.0, 0.0, 1.0);\n"
                                  "    }\n"
                                  "    gl_FragColor = color;\n"
                                  "}";

    const char *GreaterThanFS = "#version 100\n"
                                "precision mediump float;\n"
                                "uniform vec3 uVec1;\n"
                                "uniform vec3 uVec2;\n"
                                "void main() {\n"
                                "    bvec3 result = greaterThan(uVec1, uVec2);\n"
                                "    bvec3 expected = bvec3(false, false, true);\n"
                                "    vec4 color;\n"
                                "    if(result == expected){\n"
                                "        color = vec4(0.0, 1.0, 0.0, 1.0);\n"
                                "    } else{\n"
                                "        color = vec4(1.0, 0.0, 0.0, 1.0);\n"
                                "    }\n"
                                "    gl_FragColor = color;\n"
                                "}";

    const char *GreaterThanEqualFS = "#version 100\n"
                                     "precision mediump float;\n"
                                     "uniform vec3 uVec1;\n"
                                     "uniform vec3 uVec2;\n"
                                     "void main() {\n"
                                     "    bvec3 result = greaterThanEqual(uVec1, uVec2);\n"
                                     "    bvec3 expected = bvec3(false, true, true);\n"
                                     "    vec4 color;\n"
                                     "    if(result == expected){\n"
                                     "        color = vec4(0.0, 1.0, 0.0, 1.0);\n"
                                     "    } else{\n"
                                     "        color = vec4(1.0, 0.0, 0.0, 1.0);\n"
                                     "    }\n"
                                     "    gl_FragColor = color;\n"
                                     "}";

    const char *EqualFS = "#version 100\n"
                          "precision mediump float;\n"
                          "uniform vec3 uVec1;\n"
                          "uniform vec3 uVec2;\n"
                          "void main() {\n"
                          "    bvec3 result = equal(uVec1, uVec2);\n"
                          "    bvec3 expected = bvec3(true, true, true);\n"
                          "    vec4 color;\n"
                          "    if(result == expected){\n"
                          "        color = vec4(0.0, 1.0, 0.0, 1.0);\n"
                          "    } else{\n"
                          "        color = vec4(1.0, 0.0, 0.0, 1.0);\n"
                          "    }\n"
                          "    gl_FragColor = color;\n"
                          "}";

    const char *NotEqualFS = "#version 100\n"
                             "precision mediump float;\n"
                             "uniform vec3 uVec1;\n"
                             "uniform vec3 uVec2;\n"
                             "void main() {\n"
                             "    bvec3 result = notEqual(uVec1, uVec2);\n"
                             "    bvec3 expected = bvec3(true, false, true);\n"
                             "    vec4 color;\n"
                             "    if(result == expected){\n"
                             "        color = vec4(0.0, 1.0, 0.0, 1.0);\n"
                             "    } else{\n"
                             "        color = vec4(1.0, 0.0, 0.0, 1.0);\n"
                             "    }\n"
                             "    gl_FragColor = color;\n"
                             "}";

    const char *AnyFS = "#version 100\n"
                        "precision mediump float;\n"
                        "uniform vec3 uVec1;\n"
                        "uniform vec3 uVec2;\n"
                        "void main() {\n"
                        "    bvec3 comparison = greaterThan(uVec1, uVec2);\n"
                        "    bool result = any(comparison);\n"
                        "    vec4 color;\n"
                        "    if(result == true){\n"
                        "        color = vec4(0.0, 1.0, 0.0, 1.0);\n"
                        "    } else{\n"
                        "        color = vec4(1.0, 0.0, 0.0, 1.0);\n"
                        "    }\n"
                        "    gl_FragColor = color;\n"
                        "}";

    const char *AllFS = "#version 100\n"
                        "precision mediump float;\n"
                        "uniform vec3 uVec1;\n"
                        "uniform vec3 uVec2;\n"
                        "void main() {\n"
                        "    bvec3 comparison = equal(uVec1, uVec2);\n"
                        "    bool result = all(comparison);\n"
                        "    vec4 color;\n"
                        "    if(result == true){\n"
                        "        color = vec4(0.0, 1.0, 0.0, 1.0);\n"
                        "    } else{\n"
                        "        color = vec4(1.0, 0.0, 0.0, 1.0);\n"
                        "    }\n"
                        "    gl_FragColor = color;\n"
                        "}";

    const char *NotFS = "#version 100\n"
                        "precision mediump float;\n"
                        "uniform vec3 uVec1;\n"
                        "uniform vec3 uVec2;\n"
                        "void main() {\n"
                        "    bvec3 comparison = equal(uVec1, uVec2);\n"
                        "    bvec3 result = not(comparison);\n"
                        "    bvec3 expected = bvec3(false, false, false);\n"
                        "    vec4 color;\n"
                        "    if(result == expected){\n"
                        "        color = vec4(0.0, 1.0, 0.0, 1.0);\n"
                        "    } else{\n"
                        "        color = vec4(1.0, 0.0, 0.0, 1.0);\n"
                        "    }\n"
                        "    gl_FragColor = color;\n"
                        "}";

    const char *DegreesFS = "#version 100\n"
                            "precision mediump float;\n"
                            "uniform float uRadians;\n"
                            "void main() {\n"
                            "    float result = degrees(uRadians);\n"
                            "    vec4 color;\n"
                            "    if(abs(result - 90.0) < 0.1){\n"
                            "        color = vec4(0.0, 1.0, 0.0, 1.0);\n"
                            "    } else{\n"
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

    GLuint lessThanFShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(lessThanFShader, 1, &LessThanFS, NULL);
    glCompileShader(lessThanFShader);

    GLuint lessThanEqualFShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(lessThanEqualFShader, 1, &LessThanEqualFS, NULL);
    glCompileShader(lessThanEqualFShader);

    GLuint greaterThanFShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(greaterThanFShader, 1, &GreaterThanFS, NULL);
    glCompileShader(greaterThanFShader);

    GLuint greaterThanEqualFShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(greaterThanEqualFShader, 1, &GreaterThanEqualFS, NULL);
    glCompileShader(greaterThanEqualFShader);

    GLuint equalFShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(equalFShader, 1, &EqualFS, NULL);
    glCompileShader(equalFShader);

    GLuint notEqualFShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(notEqualFShader, 1, &NotEqualFS, NULL);
    glCompileShader(notEqualFShader);

    GLuint anyFShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(anyFShader, 1, &AnyFS, NULL);
    glCompileShader(anyFShader);

    GLuint allFShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(allFShader, 1, &AllFS, NULL);
    glCompileShader(allFShader);

    GLuint notFShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(notFShader, 1, &NotFS, NULL);
    glCompileShader(notFShader);

    GLuint degreesFShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(degreesFShader, 1, &DegreesFS, NULL);
    glCompileShader(degreesFShader);

    // Create shader programs
    basicProgram = glCreateProgram();
    glAttachShader(basicProgram, vertexShader);
    glAttachShader(basicProgram, basicFShader);
    glLinkProgram(basicProgram);

    lessThanProgram = glCreateProgram();
    glAttachShader(lessThanProgram, vertexShader);
    glAttachShader(lessThanProgram, lessThanFShader);
    glLinkProgram(lessThanProgram);

    lessThanEqualProgram = glCreateProgram();
    glAttachShader(lessThanEqualProgram, vertexShader);
    glAttachShader(lessThanEqualProgram, lessThanEqualFShader);
    glLinkProgram(lessThanEqualProgram);

    greaterThanProgram = glCreateProgram();
    glAttachShader(greaterThanProgram, vertexShader);
    glAttachShader(greaterThanProgram, greaterThanFShader);
    glLinkProgram(greaterThanProgram);

    greaterThanEqualProgram = glCreateProgram();
    glAttachShader(greaterThanEqualProgram, vertexShader);
    glAttachShader(greaterThanEqualProgram, greaterThanEqualFShader);
    glLinkProgram(greaterThanEqualProgram);

    equalProgram = glCreateProgram();
    glAttachShader(equalProgram, vertexShader);
    glAttachShader(equalProgram, equalFShader);
    glLinkProgram(equalProgram);

    notEqualProgram = glCreateProgram();
    glAttachShader(notEqualProgram, vertexShader);
    glAttachShader(notEqualProgram, notEqualFShader);
    glLinkProgram(notEqualProgram);

    anyProgram = glCreateProgram();
    glAttachShader(anyProgram, vertexShader);
    glAttachShader(anyProgram, anyFShader);
    glLinkProgram(anyProgram);

    allProgram = glCreateProgram();
    glAttachShader(allProgram, vertexShader);
    glAttachShader(allProgram, allFShader);
    glLinkProgram(allProgram);

    notProgram = glCreateProgram();
    glAttachShader(notProgram, vertexShader);
    glAttachShader(notProgram, notFShader);
    glLinkProgram(notProgram);

    degreesProgram = glCreateProgram();
    glAttachShader(degreesProgram, vertexShader);
    glAttachShader(degreesProgram, degreesFShader);
    glLinkProgram(degreesProgram);

    // Clean up shaders
    glDeleteShader(vertexShader);
    glDeleteShader(basicFShader);
    glDeleteShader(lessThanFShader);
    glDeleteShader(lessThanEqualFShader);
    glDeleteShader(greaterThanFShader);
    glDeleteShader(greaterThanEqualFShader);
    glDeleteShader(equalFShader);
    glDeleteShader(notEqualFShader);
    glDeleteShader(anyFShader);
    glDeleteShader(allFShader);
    glDeleteShader(notFShader);
    glDeleteShader(degreesFShader);

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
    lessThanVec1Loc = glGetUniformLocation(lessThanProgram, "uVec1");
    lessThanVec2Loc = glGetUniformLocation(lessThanProgram, "uVec2");
    lessThanEqualVec1Loc = glGetUniformLocation(lessThanEqualProgram, "uVec1");
    lessThanEqualVec2Loc = glGetUniformLocation(lessThanEqualProgram, "uVec2");
    greaterThanVec1Loc = glGetUniformLocation(greaterThanProgram, "uVec1");
    greaterThanVec2Loc = glGetUniformLocation(greaterThanProgram, "uVec2");
    greaterThanEqualVec1Loc = glGetUniformLocation(greaterThanEqualProgram, "uVec1");
    greaterThanEqualVec2Loc = glGetUniformLocation(greaterThanEqualProgram, "uVec2");
    equalVec1Loc = glGetUniformLocation(equalProgram, "uVec1");
    equalVec2Loc = glGetUniformLocation(equalProgram, "uVec2");
    notEqualVec1Loc = glGetUniformLocation(notEqualProgram, "uVec1");
    notEqualVec2Loc = glGetUniformLocation(notEqualProgram, "uVec2");
    anyVecLoc = glGetUniformLocation(anyProgram, "uVec1");
    allVecLoc = glGetUniformLocation(allProgram, "uVec1");
    notVecLoc = glGetUniformLocation(notProgram, "uVec1");
    degreesRadLoc = glGetUniformLocation(degreesProgram, "uRadians");
}

void draw() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindBuffer(GL_ARRAY_BUFFER, rectangleVBO);

    // lessThan test - (1,2,3) < (2,2,1) = (true, false, false)
    glViewport(0, (g_height/2)+5, (g_width/5)-8, (g_height/2)-5);
    glUseProgram(lessThanProgram);
    glUniform3fv(lessThanVec1Loc, 1, vec1_Compare);
    glUniform3fv(lessThanVec2Loc, 1, vec2_Compare);
    glDrawArrays(GL_TRIANGLES,0,6);

    // lessThanEqual test - (1,2,3) <= (2,2,1) = (true, true, false)
    glViewport((g_width/5)+2, (g_height/2)+5, (g_width/5)-8, (g_height/2)-5);
    glUseProgram(lessThanEqualProgram);
    glUniform3fv(lessThanEqualVec1Loc, 1, vec1_Compare);
    glUniform3fv(lessThanEqualVec2Loc, 1, vec2_Compare);
    glDrawArrays(GL_TRIANGLES,0,6);

    // greaterThan test - (1,2,3) > (2,2,1) = (false, false, true)
    glViewport((2*g_width/5)+4, (g_height/2)+5, (g_width/5)-8, (g_height/2)-5);
    glUseProgram(greaterThanProgram);
    glUniform3fv(greaterThanVec1Loc, 1, vec1_Compare);
    glUniform3fv(greaterThanVec2Loc, 1, vec2_Compare);
    glDrawArrays(GL_TRIANGLES,0,6);

    // greaterThanEqual test - (1,2,3) >= (2,2,1) = (false, true, true)
    glViewport((3*g_width/5)+6, (g_height/2)+5, (g_width/5)-8, (g_height/2)-5);
    glUseProgram(greaterThanEqualProgram);
    glUniform3fv(greaterThanEqualVec1Loc, 1, vec1_Compare);
    glUniform3fv(greaterThanEqualVec2Loc, 1, vec2_Compare);
    glDrawArrays(GL_TRIANGLES,0,6);

    // equal test - equal vectors should return all true
    glViewport((4*g_width/5)+8, (g_height/2)+5, (g_width/5)-8, (g_height/2)-5);
    glUseProgram(equalProgram);
    glUniform3fv(equalVec1Loc, 1, vec1_Equal);
    glUniform3fv(equalVec2Loc, 1, vec2_Equal);
    glDrawArrays(GL_TRIANGLES,0,6);

    // notEqual test - (1,2,3) != (2,2,1) = (true, false, true)
    glViewport(0, 0, (g_width/5)-8, (g_height/2)-5);
    glUseProgram(notEqualProgram);
    glUniform3fv(notEqualVec1Loc, 1, vec1_Compare);
    glUniform3fv(notEqualVec2Loc, 1, vec2_Compare);
    glDrawArrays(GL_TRIANGLES,0,6);

    // any test - any component of (1,2,3) > (2,2,1) is true
    glViewport((g_width/5)+2, 0, (g_width/5)-8, (g_height/2)-5);
    glUseProgram(anyProgram);
    glUniform3fv(anyVecLoc, 1, vec1_Compare);
    glUniform3fv(glGetUniformLocation(anyProgram, "uVec2"), 1, vec2_Compare);
    glDrawArrays(GL_TRIANGLES,0,6);

    // all test - all components of equal vectors are equal
    glViewport((2*g_width/5)+4, 0, (g_width/5)-8, (g_height/2)-5);
    glUseProgram(allProgram);
    glUniform3fv(allVecLoc, 1, vec1_Equal);
    glUniform3fv(glGetUniformLocation(allProgram, "uVec2"), 1, vec2_Equal);
    glDrawArrays(GL_TRIANGLES,0,6);

    // not test - logical complement of equal comparison
    glViewport((3*g_width/5)+6, 0, (g_width/5)-8, (g_height/2)-5);
    glUseProgram(notProgram);
    glUniform3fv(notVecLoc, 1, vec1_Equal);
    glUniform3fv(glGetUniformLocation(notProgram, "uVec2"), 1, vec2_Equal);
    glDrawArrays(GL_TRIANGLES,0,6);

    // degrees test - convert PI/2 radians to 90 degrees
    glViewport((4*g_width/5)+8, 0, (g_width/5)-8, (g_height/2)-5);
    glUseProgram(degreesProgram);
    glUniform1f(degreesRadLoc, radians_Test);
    glDrawArrays(GL_TRIANGLES,0,6);

    glfwSwapBuffers(window);
    glfwPollEvents();
}
