#include <GLES2/gl2.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

void init();
void drawHelper(unsigned int VBO, int size, float color[3]);
void depthTestFunc_test(GLenum type);
void draw();

int g_width = 1280, g_height = 720;

static float navy[3] = {0.0f, 0.125f, 0.376f};
static float yellow[3] = {1.0f, 0.835f, 0.0f};

static GLFWwindow *window;
static GLuint shaderProgram;
static unsigned int triangleVBO, rectangleVBO;

int main() {

    // GLFW initialization
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(g_width, g_height, "Depth Test Example", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    init();

    while (!glfwWindowShouldClose(window)) {
        draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteBuffers(1, &triangleVBO);
    glDeleteBuffers(1, &rectangleVBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void drawHelper(unsigned int VBO, int size, float color[3]) {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    GLint uColorLocation = glGetUniformLocation(shaderProgram, "uColor");
    glUniform3fv(uColorLocation,1, color);

    glDrawArrays(GL_TRIANGLES, 0, size);
}

void draw(){
    // Clear the screen
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

    //------------------------------------No Test-------------------------------------
    glViewport(0, 0, g_width/7, g_height); // [0,0]
    glDisable(GL_DEPTH_TEST);
    drawHelper(triangleVBO, 3, navy);
    drawHelper(rectangleVBO, 6, yellow);

    //------------------------------------GL_NEVER------------------------------------
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    glViewport(g_width/7, 0, g_width/7, g_height); // [0,1]
    depthTestFunc_test(GL_LESS);

    //------------------------------------GL_EQUAL------------------------------------
    glViewport((g_width/7)*2, 0, g_width/7, g_height); // [0,2]
    depthTestFunc_test( GL_EQUAL);

    //------------------------------------GL_LEQUAL------------------------------------
    glViewport((g_width/7)*3, 0, g_width/7, g_height); // [0,3]
    depthTestFunc_test( GL_LEQUAL);

    //------------------------------------GL_NOTEQUAL------------------------------------
    glViewport((g_width/7)*4, 0, g_width/7, g_height); // [0,4]
    depthTestFunc_test( GL_NOTEQUAL);

    //------------------------------------GL_GEQUAL------------------------------------
    glViewport((g_width/7)*5, 0, g_width/7, g_height); // [0,5]
    depthTestFunc_test( GL_GEQUAL);

    //------------------------------------GL_ALWAYS------------------------------------
    glViewport((g_width/7)*6, 0, g_width/7, g_height); // [0,6]
    depthTestFunc_test( GL_ALWAYS);

    glDisable(GL_DEPTH_TEST);
}

void depthTestFunc_test(GLenum type) {
    glDepthFunc(type);

    drawHelper(triangleVBO, 3, navy);
    drawHelper(rectangleVBO, 6, yellow);
}

void init() {

    // Shader source code
    const char *VSsource = "#version 100\n"
                                  "attribute vec3 aPosition;\n"
                                  "void main()\n"
                                  "{\n"
                                  "    gl_Position = vec4(aPosition, 1.0);\n"
                                  "}\n";

    const char *FSsource = "#version 100\n"
                                  "precision mediump float;\n"
                                  "uniform vec3 uColor;\n"
                                  "void main()\n"
                                  "{\n"
                                  "    gl_FragColor = vec4(uColor, 1.0);\n"
                                  "}\n";



    // Shader compilation and program creation
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &VSsource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &FSsource, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Triangle vertices
    float triangleVertices[] = {
            -0.8f, -0.16f, 1.0f, // left
            0.8f, -0.16f, 1.0f, // right
            0.0f,  0.16f, 1.0f  // top
    };

    // Square vertices
    float rectangleVertices[] = {
            -0.6f, -0.05f, 0.0f, // bottom left
            0.6f, -0.05f, 0.0f, // bottom right
            -0.6f,  0.05f, 0.0f, // top left
            0.6f, -0.05f, 0.0f, // bottom right
            0.6f,  0.05f, 0.0f, // top right
            -0.6f,  0.05f, 0.0f  // top left
    };

    // Generate and bind VBOs
    glGenBuffers(1, &triangleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &rectangleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, rectangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

    glEnable(GL_DEPTH_TEST);
}