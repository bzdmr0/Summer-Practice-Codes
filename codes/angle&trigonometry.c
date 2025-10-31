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

static GLFWwindow *window;
static GLuint basicProgram,tanProgram,arcTanProgram,arcSinArcCosProgram,sinCosRadianProgram;
static unsigned int xAndYAxisVBO,lineVBO,graphLineVBO;

static GLint decision,tanPosX,sinCosRadianAngle,arcPosX,arcTanPosX;

static int g_width = 1280, g_height = 720;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(g_width, g_height, "glsl angle & geometry", NULL, NULL);
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

    glDeleteBuffers(1, &xAndYAxisVBO);
    glDeleteBuffers(1, &lineVBO);
    glDeleteBuffers(1, &graphLineVBO);
    glDeleteProgram(basicProgram);
    glDeleteProgram(sinCosRadianProgram);
    glDeleteProgram(tanProgram);
    glDeleteProgram(arcSinArcCosProgram);
    glDeleteProgram(arcTanProgram);

    glfwTerminate();
    printf("Program terminated.\n");
    return 0;
}

void init() {

    const char * BasicVS = "#version 100\n"
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

    const char *SinCosRadianVS = "#version 100\n"
                                 "attribute vec3 aPos;\n"
                                 "uniform float uAngle;\n"
                                 "void main() {\n"
                                 "    float rad = radians(uAngle);\n"
                                 "    float y = aPos.x * sin(rad);\n"
                                 "    float x = aPos.x * cos(rad);\n"
                                 "    gl_Position = vec4(x, y, aPos.z, 1.0);\n"
                                 "}";

    const char *TanVS = "#version 100\n"
                        "attribute vec3 aPos;\n"
                        "uniform float uPosX;\n"
                        "#define PI 3.14159265359\n"
                        "void main() {\n"
                        "    float newX = aPos.x + uPosX;\n"
                        "    float radX = newX * PI;\n"
                        "    float y = tan(radX);\n"
                        "    gl_Position = vec4(newX, y, aPos.z, 1.0);\n"
                        "}";

    const char *ArcTanVS = "#version 100\n"
                           "attribute vec3 aPos;\n"
                           "uniform float uPosX;\n"
                           "#define PI 3.14159265359\n"
                           "void main() {\n"
                           "    float newX = (aPos.x + uPosX);\n"
                           "    float radY = atan(newX);\n"
                           "    float y = radY / PI;\n"
                           "    gl_Position = vec4(newX, y, aPos.z, 1.0);\n"
                           "}";

    const char *ArcSinArcCosVS = "#version 100\n"
                                 "attribute vec3 aPos;\n"
                                 "uniform float uPosX;\n"
                                 "uniform int uDecision;\n"
                                 "#define PI 3.14159265359\n"
                                 "void main() {\n"
                                 "    float radY;\n"
                                 "    float newX = aPos.x + uPosX;\n"
                                 "    if(uDecision == 0) {\n"
                                 "        radY = asin(newX);\n"
                                 "    } else {\n"
                                 "        radY = acos(newX);\n"
                                 "    }\n"
                                 "    float y = radY / PI;\n"
                                 "    gl_Position = vec4(newX, y, aPos.z, 1.0);\n"
                                 "}";

    GLuint basicVShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(basicVShader, 1, &BasicVS, NULL);
    glCompileShader(basicVShader);

    GLuint basicFShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(basicFShader, 1, &BasicFS, NULL);
    glCompileShader(basicFShader);

    GLuint tanVShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(tanVShader, 1, &TanVS, NULL);
    glCompileShader(tanVShader);

    GLuint arcTanVShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(arcTanVShader, 1, &ArcTanVS, NULL);
    glCompileShader(arcTanVShader);

    GLuint arcSinArcCosVShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(arcSinArcCosVShader, 1, &ArcSinArcCosVS, NULL);
    glCompileShader(arcSinArcCosVShader);

    GLuint sinCosRadianVShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sinCosRadianVShader, 1, &SinCosRadianVS, NULL);
    glCompileShader(sinCosRadianVShader);

    // Create shader programs
    basicProgram = glCreateProgram();
    glAttachShader(basicProgram, basicVShader);
    glAttachShader(basicProgram, basicFShader);
    glLinkProgram(basicProgram);

    tanProgram = glCreateProgram();
    glAttachShader(tanProgram, tanVShader);
    glAttachShader(tanProgram, basicFShader);
    glLinkProgram(tanProgram);

    arcTanProgram = glCreateProgram();
    glAttachShader(arcTanProgram, arcTanVShader);
    glAttachShader(arcTanProgram, basicFShader);
    glLinkProgram(arcTanProgram);

    arcSinArcCosProgram = glCreateProgram();
    glAttachShader(arcSinArcCosProgram, arcSinArcCosVShader);
    glAttachShader(arcSinArcCosProgram, basicFShader);
    glLinkProgram(arcSinArcCosProgram);

    sinCosRadianProgram = glCreateProgram();
    glAttachShader(sinCosRadianProgram, sinCosRadianVShader);
    glAttachShader(sinCosRadianProgram, basicFShader);
    glLinkProgram(sinCosRadianProgram);

    glDeleteShader(basicVShader);
    glDeleteShader(basicFShader);
    glDeleteShader(tanVShader);
    glDeleteShader(arcTanVShader);
    glDeleteShader(arcSinArcCosVShader);
    glDeleteShader(sinCosRadianVShader);

    float graphLine[] ={
            -1.0f,0.0f,0.0f,
            -0.9f,0.0f,0.0f
    };

    float xAndYAxis[] = {
            -1.0f, 0.0f, 0.0f,  // X-axis start point
            1.0f, 0.0f, 0.0f,   // X-axis end point
            0.0f, -1.0f, 0.0f,  // Y-axis start point
            0.0f, 1.0f, 0.0f    // Y-axis end point
    };

    float line[] ={
            0.0f,0.0f,0.0f,
            0.5f,0.0f,0.0f
    };

    glGenBuffers(1, &xAndYAxisVBO);
    glGenBuffers(1, &lineVBO);
    glGenBuffers(1,&graphLineVBO);

    glBindBuffer(GL_ARRAY_BUFFER, graphLineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(graphLine), graphLine, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, xAndYAxisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(xAndYAxis), xAndYAxis, GL_STATIC_DRAW);

    decision = glGetUniformLocation(arcSinArcCosProgram, "uDecision");
    tanPosX = glGetUniformLocation(tanProgram, "uPosX");
    sinCosRadianAngle = glGetUniformLocation(sinCosRadianProgram, "uAngle");
    arcPosX = glGetUniformLocation(arcSinArcCosProgram, "uPosX");
    arcTanPosX = glGetUniformLocation(arcTanProgram, "uPosX");


}

void drawLine(GLuint programID, unsigned int VBO, int size, float color[3]) {
    glUseProgram(programID);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    GLuint posAttrib = glGetAttribLocation(programID, "aPos");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(posAttrib);
    glUniform3fv(glGetUniformLocation(programID, "uColor"), 1, color);
    glDrawArrays(GL_LINES,0,size);
}

void drawAxis(GLuint program, unsigned int VBO, float color[3]){
    glUseProgram(program);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    GLuint posAttrib = glGetAttribLocation(program, "aPos");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(posAttrib);
    glUniform3fv(glGetUniformLocation(program, "uColor"), 1, color);
    glDrawArrays(GL_LINES,0,4);
}

void draw(){
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, g_height/2, g_width/2, g_height/2); // [0,0]

    drawAxis(basicProgram, xAndYAxisVBO, navy);

    for(int i = 0; i < 360; i+=5) {
        glUniform1f(sinCosRadianAngle, 1.0f * i);
        drawLine(sinCosRadianProgram,lineVBO,2, green);
        //glfwSwapBuffers(window);
        //glfwPollEvents();
    }

    //------------------------------------------------------------------------------

    glViewport(g_width/2, g_height/2, g_width/2, g_height/2); // [0,1]

    drawAxis(basicProgram, xAndYAxisVBO, yellow);

    // -pi/2 <= x <= pi/2
    for(int i = 10; i < 28; i+=1) {
        glUniform1f(tanPosX, 0.05f * i);
        drawLine(tanProgram,graphLineVBO,2, navy);
        // glfwSwapBuffers(window);
        // glfwPollEvents();
    }

    //------------------------------------------------------------------------------
    glViewport(0, 0, g_width/2, g_height/2); // [1,0]

    drawAxis(basicProgram, xAndYAxisVBO, yellow);

    // -1 <= x <= 1
    for(int i = 1; i < 38; i+=1) {
        glUniform1f(arcPosX, 0.05f * i);
        glUniform1i(decision, 1);
        drawLine(arcSinArcCosProgram, graphLineVBO, 2, navy);
        glUniform1i(decision, 0);
        drawLine(arcSinArcCosProgram, graphLineVBO, 2, green);
        //glfwSwapBuffers(window);
        //glfwPollEvents();
    }

    //------------------------------------------------------------------------------

    glViewport(g_width/2, 0, g_width/2, g_height/2); // [1,1]

    drawAxis(basicProgram, xAndYAxisVBO, green);

    // -1 <= x <= 1
    for(int i = 0; i < 40; i+=1) {
        glUniform1f(arcTanPosX, 0.05f * i);
        drawLine(arcTanProgram,graphLineVBO,2, navy);

        //glfwSwapBuffers(window);
        //glfwPollEvents();
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}
