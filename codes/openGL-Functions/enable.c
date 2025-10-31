#include <GLES2/gl2.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>

void init();
void drawHelper(unsigned int VBO, int size, float color[3], float alpha);
void draw();

int g_width = 1280, g_height = 720;

static float navy[3] = {0.0f, 0.125f, 0.376f};
static float yellow[3] = {1.0f, 0.835f, 0.0f};
static float green[3] = {0.0f, 0.65f, 0.2f};

static GLFWwindow *window;
static GLuint shaderProgram;

static unsigned int triangleVBO, rectangleVBO, littleTriangleVBO;

int main() {

    // GLFW initialization
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SAMPLES, 4);

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
    }

    // Cleanup
    glDeleteBuffers(1, &triangleVBO);
    glDeleteBuffers(1, &rectangleVBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void init() {
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(g_width, g_height, "glEnable Test", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);

    // Fragment shader with uniform color control
    static const char *FSsource = "#version 100\n"
                                  "precision mediump float;\n"
                                  "uniform vec3 uColor;\n"
                                  "uniform float uAlpha;\n"
                                  "void main()\n"
                                  "{\n"
                                  "    gl_FragColor = vec4(uColor, uAlpha);\n"
                                  "}\n";

    // Vertex shader
    static const char *VSsource = "#version 100\n"
                                  "attribute vec3 aPos;\n"
                                  "\n"
                                  "void main()\n"
                                  "{\n"
                                  "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                  "}";

    // Triangle vertices
    float triangleVertices[] = {
            -0.6f, -0.6f, 0.0f, // left
            0.6f, -0.6f, 0.0f, // right
            0.0f,  0.6f, 0.0f  // top
    };

    // Square vertices
    float rectangleVertices[] = {
            -0.6f, -0.3f, 0.0f, // bottom left
            0.6f, -0.3f, 0.0f, // bottom right
            -0.6f,  0.3f, 0.0f, // top left
            0.6f, -0.3f, 0.0f, // bottom right
            0.6f,  0.3f, 0.0f, // top right
            -0.6f,  0.3f, 0.0f  // top left
    };

    // Little triangle vertices
    float littleTriangleVertices[] = {
            -0.4f, -0.4f, 0.0f,  // left
            0.0f,  0.4f, 0.0f,  // top
            0.4f, -0.4f, 0.0f    // right
    };

    // Create shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &VSsource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1,&FSsource,NULL);
    glCompileShader(fragmentShader);

    // Create program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram,vertexShader);
    glAttachShader(shaderProgram,fragmentShader);
    glLinkProgram(shaderProgram);

    // Create buffers
    glGenBuffers(1, &triangleVBO);
    glGenBuffers(1, &rectangleVBO);
    glGenBuffers(1, &littleTriangleVBO);

    // Bind and set triangle buffer data
    glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

    // Bind and set rectangle buffer data
    glBindBuffer(GL_ARRAY_BUFFER, rectangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

    // Bind and set rectangle buffer data
    glBindBuffer(GL_ARRAY_BUFFER, littleTriangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(littleTriangleVertices), littleTriangleVertices, GL_STATIC_DRAW);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void drawHelper(unsigned int VBO, int size, float color[3], float alpha) {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glUniform3fv(glGetUniformLocation(shaderProgram, "uColor"), 1, color);
    glUniform1f(glGetUniformLocation(shaderProgram, "uAlpha"), alpha);

    glDrawArrays(GL_TRIANGLES, 0, size);
}

void draw(){
    // Clear the screen
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

    //------------------------------------No Test-------------------------------------
    glViewport(0, 0, g_width/2, g_height); // [0,0]
    drawHelper(triangleVBO, 3, navy, 1.0f); // Draw the triangle
    drawHelper(rectangleVBO, 6, yellow, 1.0f); // Draw the rectangle
    drawHelper(littleTriangleVBO, 3, green, 1.0f); // Draw the little triangle

    //------------------------------------GL_SAMPLE_ALPHA_TO_COVERAGE------------------------------------
    glViewport(g_width/2, 0, g_width/2, g_height); // [0,1]
    glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

    drawHelper(triangleVBO, 3, navy, 0.2f); // Draw the triangle
    drawHelper(rectangleVBO, 6, yellow, 0.5f); // Draw the rectangle
    drawHelper(littleTriangleVBO, 3, green, 1.0f); // Draw the little triangle

    glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);

    // Swap buffers and poll events
    glfwSwapBuffers(window);
    glfwPollEvents();
}

