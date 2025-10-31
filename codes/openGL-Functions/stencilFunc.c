#include <GLES2/gl2.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

void init();
void draw();

static int g_width = 1280, g_height = 720;

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

        // Swap buffers and poll events
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

void init() {
    // Fragment shader with uniform color control
    const char *FSsource = "#version 100\n"
                                  "precision mediump float;\n"
                                  "uniform vec3 uColor;\n"
                                  "void main()\n"
                                  "{\n"
                                  "    gl_FragColor = vec4(uColor, 1.0);\n"
                                  "}\n";

    // Vertex shader
    const char *VSsource = "#version 100\n"
                                  "attribute vec3 aPos;\n"
                                  "\n"
                                  "void main()\n"
                                  "{\n"
                                  "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                  "}";

    // Triangle vertices
    float triangleVertices[] = {
            -0.8f, -0.16f, 0.0f, // left
            0.8f, -0.16f, 0.0f, // right
            0.0f,  0.16f, 0.0f  // top
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

    // Bind and set triangle buffer data
    glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

    // Bind and set rectangle buffer data
    glBindBuffer(GL_ARRAY_BUFFER, rectangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glEnableVertexAttribArray(0);
}

void draw(){
    // Clear the screen
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClearStencil(5); // Clear stencil buffer with 5
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

    //------------------------------------No Test-------------------------------------

    glDisable(GL_STENCIL_TEST);
    glViewport(0, 0, g_width/6, g_height); // [0,0]

    // Draw the triangle
    glBindBuffer(GL_ARRAY_BUFFER,triangleVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glUniform3fv(glGetUniformLocation(shaderProgram, "uColor"), 1, navy);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Draw the rectangle
    glBindBuffer(GL_ARRAY_BUFFER,rectangleVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glUniform3fv(glGetUniformLocation(shaderProgram, "uColor"), 1, yellow);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    //------------------------------------GL_NEVER------------------------------------

    glEnable(GL_STENCIL_TEST);
    glViewport(g_width/6, 0, g_width/6, g_height); // [0,1]

    // Set the stencil value to 1 for the triangle
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    // Draw the triangle
    glBindBuffer(GL_ARRAY_BUFFER,triangleVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glUniform3fv(glGetUniformLocation(shaderProgram, "uColor"), 1, navy);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Stencil values of new fragments will not update the stencil buffer because they will always fail the stencil test
    glStencilFunc(GL_NEVER, 1, 0xFF);
    // Draw the rectangle
    glBindBuffer(GL_ARRAY_BUFFER,rectangleVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glUniform3fv(glGetUniformLocation(shaderProgram, "uColor"), 1, yellow);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    //------------------------------------GL_LESS------------------------------------

    glViewport((g_width/6)*2, 0, g_width/6, g_height); // [0,2]

    // Set the stencil value to 1 for the triangle
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    // Draw the triangle
    glBindBuffer(GL_ARRAY_BUFFER,triangleVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glUniform3fv(glGetUniformLocation(shaderProgram, "uColor"), 1, navy);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Stencil values of new fragments will not pass the stencil test in triangle area but
    // they will pass outside the triangle area
    // test-------- 1<3<5
    glStencilFunc(GL_LESS, 3, 0xFF);
    // Draw the rectangle
    glBindBuffer(GL_ARRAY_BUFFER,rectangleVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glUniform3fv(glGetUniformLocation(shaderProgram, "uColor"), 1, yellow);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    //------------------------------------GL_LEQUAL------------------------------------

    glViewport((g_width/6)*3, 0, g_width/6, g_height); // [0,3]

    // Set the stencil value to 1 for the triangle
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    // Draw the triangle
    glBindBuffer(GL_ARRAY_BUFFER,triangleVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glUniform3fv(glGetUniformLocation(shaderProgram, "uColor"), 1, navy);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Stencil values of new fragments will pass stencil test in both triangle
    // and rectangle areas because they will pass the stencil
    // test-------- 1<=1<=5
    glStencilFunc(GL_LEQUAL, 1, 0xFF);
    // Draw the rectangle
    glBindBuffer(GL_ARRAY_BUFFER,rectangleVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glUniform3fv(glGetUniformLocation(shaderProgram, "uColor"), 1, yellow);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    //------------------------------------GL_GREATER------------------------------------

    glViewport((g_width/6)*4, 0, g_width/6, g_height); // [0,4]

    // Set the stencil value to 1 for the triangle
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    // Draw the triangle
    glBindBuffer(GL_ARRAY_BUFFER,triangleVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glUniform3fv(glGetUniformLocation(shaderProgram, "uColor"), 1, navy);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Stencil values of new fragments will pass the stencil test in triangle area but
    // they will not pass outside the triangle area
    // test-------- 1<3<5
    glStencilFunc(GL_GREATER, 3, 0xFF);
    // Draw the rectangle
    glBindBuffer(GL_ARRAY_BUFFER,rectangleVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glUniform3fv(glGetUniformLocation(shaderProgram, "uColor"), 1, yellow);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    //------------------------------------GL_GEQUAL------------------------------------

    glViewport((g_width/6)*5, 0, g_width/6, g_height); // [0,5]

    // Set the stencil value to 1 for the triangle
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    // Draw the triangle
    glBindBuffer(GL_ARRAY_BUFFER,triangleVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glUniform3fv(glGetUniformLocation(shaderProgram, "uColor"), 1, navy);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Stencil values of new fragments will pass stencil test in both triangle
    // and rectangle areas because they will pass the stencil
    // test-------- 1<=5<=5
    glStencilFunc(GL_GEQUAL, 5, 0xFF);
    // Draw the rectangle
    glBindBuffer(GL_ARRAY_BUFFER,rectangleVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glUniform3fv(glGetUniformLocation(shaderProgram, "uColor"), 1, yellow);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisable(GL_STENCIL_TEST);
}
