#include <GLES2/gl2.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

void init();
void drawHelper(unsigned int VBO, int size, float color[3]);
void GL_KEEP_test();
void GL_ZERO_test();
void GL_REPLACE_test();
void GL_INCR_test();
void GL_DECR_test();
void GL_INVERT_test();
void GL_INCR_WRAP_test();
void GL_DECR_WRAP_test();
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

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteBuffers(1, &triangleVBO);
    glDeleteBuffers(1, &rectangleVBO);
    glDeleteBuffers(1, &littleTriangleVBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    printf("Program terminated.\n");
    return 0;
}

void drawHelper(unsigned int VBO, int size, float color[3]) {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    GLint uColorLocation = glGetUniformLocation(shaderProgram, "uColor");
    glUniform3f(uColorLocation, color[0], color[1], color[2]);

    glDrawArrays(GL_TRIANGLES, 0, size);
}

void draw() {
    // Clear the screen
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClearStencil(5);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

    //------------------------------------------No Test------------------------------------------
    glDisable(GL_STENCIL_TEST);
    glViewport(0, (g_height/3)*2, g_width/3, g_height/3);

    drawHelper(triangleVBO, 3, navy);
    drawHelper(rectangleVBO, 6, yellow);
    drawHelper(littleTriangleVBO, 3, green);


    glEnable(GL_STENCIL_TEST);
    //------------------------------------------GL_KEEP------------------------------------------
    GL_KEEP_test();

    //------------------------------------------GL_ZERO------------------------------------------
    GL_ZERO_test();

    //------------------------------------------GL_REPLACE------------------------------------------
    GL_REPLACE_test();

    //------------------------------------------GL_INCR------------------------------------------
    GL_INCR_test();

    //------------------------------------------GL_DECR------------------------------------------
    GL_DECR_test();

    //------------------------------------------GL_INVERT------------------------------------------
    GL_INVERT_test();

    //------------------------------------------GL_INCR_WRAP------------------------------------------
    GL_INCR_WRAP_test();

    //------------------------------------------GL_DECR_WRAP------------------------------------------
    GL_DECR_WRAP_test();

    glDisable(GL_STENCIL_TEST);
}

void GL_KEEP_test() {
    glViewport(g_width/3, (g_height/3)*2, g_width/3, g_height/3);

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    drawHelper(triangleVBO, 3, navy);

    glStencilFunc(GL_EQUAL, 5, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    drawHelper(rectangleVBO, 6, yellow);

    glStencilFunc(GL_EQUAL, 5, 0xFF);
    drawHelper(littleTriangleVBO, 3, green);
}

void GL_ZERO_test() {
    glViewport((g_width/3)*2, (g_height/3)*2, g_width/3, g_height/3);

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
    drawHelper(triangleVBO, 3, navy);

    glStencilFunc(GL_EQUAL, 5, 0xFF);
    glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
    drawHelper(rectangleVBO, 6, yellow);

    glStencilFunc(GL_EQUAL, 0, 0xFF);
    drawHelper(littleTriangleVBO, 3, green);
}

void GL_REPLACE_test() {
    glViewport(0, g_height/3, g_width/3, g_height/3);

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    drawHelper(triangleVBO, 3, navy);

    glStencilFunc(GL_EQUAL, 1, 0xFF);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    drawHelper(rectangleVBO, 6, yellow);

    glStencilFunc(GL_EQUAL, 1, 0xFF);
    drawHelper(littleTriangleVBO, 3, green);
}

void GL_INCR_test() {
    glClearStencil(254);
    glClear(GL_STENCIL_BUFFER_BIT);

    glViewport(g_width/3, g_height/3, g_width/3, g_height/3);

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_INCR, GL_INCR, GL_INCR);
    drawHelper(triangleVBO, 3, navy);

    glStencilFunc(GL_EQUAL, 255, 0xFF);
    glStencilOp(GL_INCR, GL_INCR, GL_INCR);
    drawHelper(rectangleVBO, 6, yellow);

    glStencilFunc(GL_EQUAL, 0, 0xFF);
    drawHelper(littleTriangleVBO, 3, green);
}

void GL_DECR_test() {
    glClearStencil(1);
    glClear(GL_STENCIL_BUFFER_BIT);

    glViewport((g_width/3)*2, g_height/3, g_width/3, g_height/3);

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_DECR, GL_DECR, GL_DECR);
    drawHelper(triangleVBO, 3, navy);

    glStencilFunc(GL_EQUAL, 0, 0xFF);
    glStencilOp(GL_DECR, GL_DECR, GL_DECR);
    drawHelper(rectangleVBO, 6, yellow);

    glStencilFunc(GL_EQUAL, 255, 0xFF);
    drawHelper(littleTriangleVBO, 3, green);
}

void GL_INVERT_test() {
    glClearStencil(5);
    glClear(GL_STENCIL_BUFFER_BIT);

    glViewport(0, 0, g_width/3, g_height/3);

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);
    drawHelper(triangleVBO, 3, navy);

    glStencilFunc(GL_EQUAL, 250, 0xFF);
    glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);
    drawHelper(rectangleVBO, 6, yellow);

    glStencilFunc(GL_EQUAL, 5, 0xFF);
    drawHelper(littleTriangleVBO, 3, green);
}

void GL_INCR_WRAP_test() {
    glClearStencil(255);
    glClear(GL_STENCIL_BUFFER_BIT);

    glViewport(g_width/3, 0, g_width/3, g_height/3);

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_INCR_WRAP, GL_INCR_WRAP, GL_INCR_WRAP);
    drawHelper(triangleVBO, 3, navy);

    glStencilFunc(GL_EQUAL, 0, 0xFF);
    glStencilOp(GL_INCR_WRAP, GL_INCR_WRAP, GL_INCR_WRAP);
    drawHelper(rectangleVBO, 6, yellow);

    glStencilFunc(GL_EQUAL, 1, 0xFF);
    drawHelper(littleTriangleVBO, 3, green);
}

void GL_DECR_WRAP_test() {
    glClearStencil(1);
    glClear(GL_STENCIL_BUFFER_BIT);

    glViewport((g_width/3)*2, 0, g_width/3, g_height/3);

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_DECR_WRAP, GL_DECR_WRAP, GL_DECR_WRAP);
    drawHelper(triangleVBO, 3, navy);

    glStencilFunc(GL_EQUAL, 0, 0xFF);
    glStencilOp(GL_DECR_WRAP, GL_DECR_WRAP, GL_DECR_WRAP);
    drawHelper(rectangleVBO, 6, yellow);

    glStencilFunc(GL_EQUAL, 255, 0xFF);
    drawHelper(littleTriangleVBO, 3, green);
}

void init() {

    // Shader source codes
    const char *VSsource = "#version 100\n"
                           "attribute vec3 aPos;\n"
                           "void main()\n"
                           "{\n"
                           "    gl_Position = vec4(aPos, 1.0);\n"
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

    // Vertex data
    float triangleVertices[] = {
            -0.6f, -0.6f, 0.0f,
            0.6f, -0.6f, 0.0f,
            0.0f,  0.6f, 0.0f
    };

    float littleTriangleVertices[] = {
            -0.5f, 0.0f, 0.0f,
            0.0f, 0.5f, 0.0f,
            0.5f, 0.0f, 0.0f
    };

    float rectangleVertices[] = {
            -0.5f, -0.2f, 0.0f,
            0.5f, -0.2f, 0.0f,
            -0.5f,  0.2f, 0.0f,
            0.5f, -0.2f, 0.0f,
            0.5f,  0.2f, 0.0f,
            -0.5f,  0.2f, 0.0f
    };

    // Generate and bind VBOs
    glGenBuffers(1, &triangleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &rectangleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, rectangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &littleTriangleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, littleTriangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(littleTriangleVertices), littleTriangleVertices, GL_STATIC_DRAW);
}