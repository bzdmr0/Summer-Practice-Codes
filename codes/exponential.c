#include <GLFW/glfw3.h>
#include <GLES2/gl2.h>
#include <stdio.h>
#include <stdlib.h>

void init();
void drawLine(GLuint programID, unsigned int VBO, int size, float color[3]);
void drawAxis(GLuint program, unsigned int VBO, float color[3]);
void draw();

static float navy[3] = {0.0f, 0.125f, 0.376f};
static float yellow[3] = {1.0f, 0.835f, 0.0f};
static float green[3] = {0.0f, 0.65f, 0.2f};
static float black[3] = {0.0f, 0.0f, 0.0f};
static float red[3] = {1.0f, 0.0f, 0.0f};

static GLFWwindow *window;
static GLuint basicProgram, powProgram, expProgram, logProgram, sqrtProgram;
static unsigned int triangleVBO, rectangleVBO, littleTriangleVBO, xAndYAxisVBO, lineVBO, graphLineVBO;

static GLint powX, expX, expType, logType, logX, sqrtX, sqrtType;

static int g_width = 1400, g_height = 700;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(g_width, g_height, "GLSL Exponential Functions", NULL, NULL);
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

    glDeleteBuffers(1, &triangleVBO);
    glDeleteBuffers(1, &rectangleVBO);
    glDeleteBuffers(1, &littleTriangleVBO);
    glDeleteBuffers(1, &xAndYAxisVBO);
    glDeleteBuffers(1, &lineVBO);
    glDeleteBuffers(1, &graphLineVBO);
    glDeleteProgram(basicProgram);
    glDeleteProgram(powProgram);
    glDeleteProgram(expProgram);
    glDeleteProgram(logProgram);
    glDeleteProgram(sqrtProgram);

    glfwTerminate();
    printf("Program terminated.\n");
    return 0;
}

void init() {
    const char *BasicVS = "#version 100\n"
                          "attribute vec3 aPos;\n"
                          "void main() {\n"
                          "    gl_Position = vec4(aPos, 1.0);\n"
                          "}";

    const char *BasicFS = "#version 100\n"
                          "precision mediump float;\n"
                          "uniform vec3 uColor;\n"
                          "void main() {\n"
                          "    gl_FragColor = vec4(uColor, 1.0);\n"
                          "}";

    const char *PowerVS = "#version 100\n"
                          "attribute vec3 aPos;\n"
                          "uniform float uPosX;\n"
                          "void main() {\n"
                          "    float newX = aPos.x + uPosX;\n"
                          "    float powY = pow(newX, 3.0);\n"
                          "    gl_Position = vec4(newX, powY, aPos.z, 1.0);\n"
                          "}";

    const char *ExpEuVS = "#version 100\n"
                          "attribute vec3 aPos;\n"
                          "uniform float uPosX;\n"
                          "uniform int uExpType;\n"
                          "void main() {\n"
                          "    float expY = 0.0;\n"
                          "    float newX = aPos.x + uPosX;\n"
                          "    if(uExpType == 2) {\n"
                          "        expY = exp2(newX)/2.0;\n"
                          "    } else {\n"
                          "        expY = exp(newX)/2.0;\n"
                          "    }\n"
                          "    gl_Position = vec4(newX, expY, aPos.z, 1.0);\n"
                          "}";

    const char *LogVS = "#version 100\n"
                        "attribute vec3 aPos;\n"
                        "uniform float uPosX;\n"
                        "uniform int uLogType;\n"
                        "void main() {\n"
                        "    float logY = 0.0;\n"
                        "    float newX = aPos.x + uPosX;\n"
                        "    if(uLogType == 2) {\n"
                        "        logY = log2(newX);\n"
                        "    } else {\n"
                        "        logY = log(newX);\n"
                        "    }\n"
                        "    newX = newX / 2.0;\n"
                        "    gl_Position = vec4(newX, logY, aPos.z, 1.0);\n"
                        "}";

    const char *SqrtVS = "#version 100\n"
                         "attribute vec3 aPos;\n"
                         "uniform float uPosX;\n"
                         "uniform int uSqrtType;\n"
                         "void main() {\n"
                         "    float sqrtY = 0.0;\n"
                         "    float newX = aPos.x + uPosX;\n"
                         "    if (newX < 0.0) {\n"
                         "        sqrtY = 0.0;\n"
                         "    } else {\n"
                         "        if (uSqrtType == 1) {\n"
                         "            sqrtY = sqrt(newX)/2.0;\n"
                         "        } else {\n"
                         "            sqrtY = inversesqrt(newX)/2.0;\n"
                         "        }\n"
                         "    }\n"
                         "    newX = newX / 2.0;\n"
                         "    gl_Position = vec4(newX, sqrtY, aPos.z, 1.0);\n"
                         "}";

    GLuint basicVShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(basicVShader, 1, &BasicVS, NULL);
    glCompileShader(basicVShader);

    GLuint basicFShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(basicFShader, 1, &BasicFS, NULL);
    glCompileShader(basicFShader);

    GLuint powerVShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(powerVShader, 1, &PowerVS, NULL);
    glCompileShader(powerVShader);

    GLuint expEuVShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(expEuVShader, 1, &ExpEuVS, NULL);
    glCompileShader(expEuVShader);

    GLuint logVShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(logVShader, 1, &LogVS, NULL);
    glCompileShader(logVShader);

    GLuint sqrtVShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sqrtVShader, 1, &SqrtVS, NULL);
    glCompileShader(sqrtVShader);

    // Create shader programs
    basicProgram = glCreateProgram();
    glAttachShader(basicProgram, basicVShader);
    glAttachShader(basicProgram, basicFShader);
    glLinkProgram(basicProgram);

    powProgram = glCreateProgram();
    glAttachShader(powProgram, powerVShader);
    glAttachShader(powProgram, basicFShader);
    glLinkProgram(powProgram);

    expProgram = glCreateProgram();
    glAttachShader(expProgram, expEuVShader);
    glAttachShader(expProgram, basicFShader);
    glLinkProgram(expProgram);

    logProgram = glCreateProgram();
    glAttachShader(logProgram, logVShader);
    glAttachShader(logProgram, basicFShader);
    glLinkProgram(logProgram);

    sqrtProgram = glCreateProgram();
    glAttachShader(sqrtProgram, sqrtVShader);
    glAttachShader(sqrtProgram, basicFShader);
    glLinkProgram(sqrtProgram);

    glDeleteShader(basicVShader);
    glDeleteShader(basicFShader);
    glDeleteShader(powerVShader);
    glDeleteShader(expEuVShader);
    glDeleteShader(logVShader);
    glDeleteShader(sqrtVShader);

    float graphLine[] = {
        -1.0f, 0.0f, 0.0f,
        -0.95f, 0.0f, 0.0f
    };

    // Triangle vertices
    float triangleVertices[] = {
        -0.61f, -0.6f, 0.0f, // left
        0.6f, -0.6f, 0.0f, // right
        0.0f,  0.6f, 0.0f  // top
    };

    float xAndYAxis[] = {
        -1.0f, 0.0f, 0.0f,  // X-axis start point
        1.0f, 0.0f, 0.0f,   // X-axis end point
        0.0f, -1.0f, 0.0f,  // Y-axis start point
        0.0f, 1.0f, 0.0f    // Y-axis end point
    };

    // Rectangle vertices
    float rectangleVertices[] = {
        -0.2f, -0.2f, 0.0f, // bottom left
        0.2f, -0.2f, 0.0f, // bottom right
        -0.2f,  0.2f, 0.0f, // top left
        0.2f, -0.2f, 0.0f, // bottom right
        0.2f,  0.2f, 0.0f, // top right
        -0.2f,  0.2f, 0.0f  // top left
    };

    float line[] = {
        0.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f
    };

    // Little triangle vertices
    float littleTriangleVertices[] = {
        -0.5f, -0.5f, 0.0f, // left
        0.5f, -0.5f, 0.0f, // right
        0.0f,  0.5f, 0.0f  // top
    };

    glGenBuffers(1, &triangleVBO);
    glGenBuffers(1, &rectangleVBO);
    glGenBuffers(1, &littleTriangleVBO);
    glGenBuffers(1, &xAndYAxisVBO);
    glGenBuffers(1, &lineVBO);
    glGenBuffers(1, &graphLineVBO);

    glBindBuffer(GL_ARRAY_BUFFER, graphLineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(graphLine), graphLine, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, rectangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, littleTriangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(littleTriangleVertices), littleTriangleVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, xAndYAxisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(xAndYAxis), xAndYAxis, GL_STATIC_DRAW);

    powX = glGetUniformLocation(powProgram, "uPosX");
    expX = glGetUniformLocation(expProgram, "uPosX");
    expType = glGetUniformLocation(expProgram, "uExpType");
    logType = glGetUniformLocation(logProgram, "uLogType");
    logX = glGetUniformLocation(logProgram, "uPosX");
    sqrtX = glGetUniformLocation(sqrtProgram, "uPosX");
    sqrtType = glGetUniformLocation(sqrtProgram, "uSqrtType");
}

void drawLine(GLuint programID, unsigned int VBO, int size, float color[3]) {
    glUseProgram(programID);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    GLuint posAttrib = glGetAttribLocation(programID, "aPos");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(posAttrib);
    glUniform3fv(glGetUniformLocation(programID, "uColor"), 1, color);
    glDrawArrays(GL_LINES, 0, size);
}

void drawAxis(GLuint program, unsigned int VBO, float color[3]) {
    glUseProgram(program);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    GLuint posAttrib = glGetAttribLocation(program, "aPos");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(posAttrib);
    glUniform3fv(glGetUniformLocation(program, "uColor"), 1, color);
    glDrawArrays(GL_LINES, 0, 4);
}

void draw() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, g_height/2, g_width/2, g_height/2); // [0,0]

    drawAxis(basicProgram, xAndYAxisVBO, black);

    // -1 <= x <= 1
    for(int i = 0; i < 40; i+=1) {
        glUniform1f(powX, 0.05f * i);
        drawLine(powProgram, graphLineVBO, 2, yellow);
    }

    glViewport(g_width/2, g_height/2, g_width/2, g_height/2); // [0,1]

    drawAxis(basicProgram, xAndYAxisVBO, red);

    // -1 <= x <= 1
    for(int i = 0; i < 40; i+=1) {
        glUniform1f(expX, 0.05f * i);
        // e^x function
        glUniform1i(expType, 0);
        drawLine(expProgram, graphLineVBO, 2, green);

        // 2^x function
        glUniform1i(expType, 2);
        drawLine(expProgram, graphLineVBO, 2, yellow);
    }

    glViewport(0, 0, g_width/2, g_height/2); // [1,0]

    drawAxis(basicProgram, xAndYAxisVBO, red);

    // -1 <= x <= 1
    // x values are divided into 2 in the shader so loop is 81
    for(int i = 0; i < 81; i+=1) {
        glUniform1f(logX, 0.05f * i);
        // Ln function
        glUniform1i(logType, 0);
        drawLine(logProgram, graphLineVBO, 2, green);

        // Log2 function
        glUniform1i(logType, 2);
        drawLine(logProgram, graphLineVBO, 2, yellow);
    }

    glViewport(g_width/2, 0, g_width/2, g_height/2); // [1,1]

    drawAxis(basicProgram, xAndYAxisVBO, black);

    // -1 <= x <= 1
    for(int i = 19; i < 80; i+=1) {
        glUniform1f(sqrtX, 0.05f * i);
        // Sqrt function
        glUniform1i(sqrtType, 1);
        drawLine(sqrtProgram, graphLineVBO, 2, yellow);

        // Inverse Sqrt function
        glUniform1i(sqrtType, 0);
        drawLine(sqrtProgram, graphLineVBO, 2, green);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}
