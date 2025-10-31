#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GLFW/glfw3.h>
#include <GLES2/gl2.h>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 900
#define TEST_COUNT 12
#define GRID_COLS 4
#define GRID_ROWS 3

void init();
void draw();
void renderTest(int testIndex);

// Static global variables
static GLFWwindow* window;
static GLuint vbo;
static int passedTests = 0;

// Quad vertices (position + texture coordinates)
static const float quadVertices[] = {
    -1.0f, -1.0f, 0.0f, 0.0f,  // bottom left
    1.0f, -1.0f, 1.0f, 0.0f,  // bottom right
    -1.0f,  1.0f, 0.0f, 1.0f,  // top left
    1.0f,  1.0f, 1.0f, 1.0f   // top right
};

// Test program and uniform arrays (replacing the struct)
static GLuint programs[TEST_COUNT];

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        printf("Failed to initialize GLFW\n");
        exit(-1);
    }

    // Configure GLFW for OpenGL ES 2.0
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

    // Create window
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "GLSL Common Functions Grid", NULL, NULL);

    // Make context current
    glfwMakeContextCurrent(window);

    init();

    while (!glfwWindowShouldClose(window)) {
        draw();
    }

    // Cleanup
    glDeleteBuffers(1, &vbo);

    for (int i = 0; i < TEST_COUNT; i++) {
        glDeleteProgram(programs[i]);
    }

    glfwTerminate();

    return 0;
}

void init() {

    // Embedded shader strings
    const char* vertexShaderSource =
        "#version 100\n"
        "attribute vec2 a_position;\n"
        "attribute vec2 a_texCoord;\n"
        "varying vec2 v_texCoord;\n"
        "\n"
        "void main() {\n"
        "    gl_Position = vec4(a_position, 0.0, 1.0);\n"
        "    v_texCoord = a_texCoord;\n"
        "}\n";

    const char* absFragmentShader =
        "#version 100\n"
        "precision mediump float;\n"
        "varying vec2 v_texCoord;\n"
        "\n"
        "void main() {\n"
        "    vec2 uv = v_texCoord;\n"
        "    float x = (uv.x - 0.5) * 5.0;\n"
        "    float result = abs(x);\n"
        "    float normalized = result / (2.5);\n"
        "    gl_FragColor = vec4(normalized, normalized, normalized, 1.0);\n"
        "}\n";

    const char* signFragmentShader =
        "#version 100\n"
        "precision mediump float;\n"
        "varying vec2 v_texCoord;\n"
        "\n"
        "void main() {\n"
        "    vec2 uv = v_texCoord;\n"
        "    float x = (uv.x - 0.5) * 5.0;\n"
        "    float result = sign(x);\n"
        "    float normalized = (result + 1.0) * 0.5;\n"
        "    gl_FragColor = vec4(normalized, normalized, normalized, 1.0);\n"
        "}\n";

    const char* floorFragmentShader =
        "#version 100\n"
        "precision mediump float;\n"
        "varying vec2 v_texCoord;\n"
        "\n"
        "void main() {\n"
        "    vec2 uv = v_texCoord;\n"
        "    float x = uv.x * 5.0;\n"
        "    float result = floor(x);\n"
        "    float normalized = mod(result, 4.0) / 4.0;\n"
        "    gl_FragColor = vec4(normalized, normalized, normalized, 1.0);\n"
        "}\n";

    const char* ceilFragmentShader =
        "#version 100\n"
        "precision mediump float;\n"
        "varying vec2 v_texCoord;\n"
        "\n"
        "void main() {\n"
        "    vec2 uv = v_texCoord;\n"
        "    float x = uv.x * 5.0;\n"
        "    float result = ceil(x);\n"
        "    float normalized = mod(result, 4.0) / 4.0;\n"
        "    gl_FragColor = vec4(normalized, normalized, normalized, 1.0);\n"
        "}\n";

    const char* fractFragmentShader =
        "#version 100\n"
        "precision mediump float;\n"
        "varying vec2 v_texCoord;\n"
        "\n"
        "void main() {\n"
        "    vec2 uv = v_texCoord;\n"
        "    float x = uv.x * 5.0;\n"
        "    float result = fract(x);\n"
        "    gl_FragColor = vec4(result, result, result, 1.0);\n"
        "}\n";

    const char* modFragmentShader =
        "#version 100\n"
        "precision mediump float;\n"
        "varying vec2 v_texCoord;\n"
        "\n"
        "void main() {\n"
        "    vec2 uv = v_texCoord;\n"
        "    float x = uv.x * 5.0;\n"
        "    float y = 2.0;\n"
        "    float result = mod(x, y);\n"
        "    float normalized = result / y;\n"
        "    gl_FragColor = vec4(normalized, normalized, normalized, 1.0);\n"
        "}\n";

    const char* minFragmentShader =
        "#version 100\n"
        "precision mediump float;\n"
        "varying vec2 v_texCoord;\n"
        "\n"
        "void main() {\n"
        "    vec2 uv = v_texCoord;\n"
        "    float result = min(uv.x, uv.y);\n"
        "    gl_FragColor = vec4(result, result, result, 1.0);\n"
        "}\n";

    const char* maxFragmentShader =
        "#version 100\n"
        "precision mediump float;\n"
        "varying vec2 v_texCoord;\n"
        "\n"
        "void main() {\n"
        "    vec2 uv = v_texCoord;\n"
        "    float result = max(uv.x, uv.y);\n"
        "    gl_FragColor = vec4(result, result, result, 1.0);\n"
        "}\n";

    const char* clampFragmentShader =
        "#version 100\n"
        "precision mediump float;\n"
        "varying vec2 v_texCoord;\n"
        "\n"
        "void main() {\n"
        "    vec2 uv = v_texCoord;\n"
        "    float result = clamp(uv.x, 0.2, 0.8);\n"
        "    gl_FragColor = vec4(result, result, result, 1.0);\n"
        "}\n";

    const char* mixFragmentShader =
        "#version 100\n"
        "precision mediump float;\n"
        "varying vec2 v_texCoord;\n"
        "\n"
        "void main() {\n"
        "    vec2 uv = v_texCoord;\n"
        "    float result = mix(uv.x, uv.y, 0.5);\n"
        "    gl_FragColor = vec4(result, result, result, 1.0);\n"
        "}\n";

    const char* stepFragmentShader =
        "#version 100\n"
        "precision mediump float;\n"
        "varying vec2 v_texCoord;\n"
        "\n"
        "void main() {\n"
        "    vec2 uv = v_texCoord;\n"
        "    float x = uv.x;\n"
        "    float edge = 0.5;\n"
        "    float result = step(edge, x);\n"
        "    gl_FragColor = vec4(result, result, result, 1.0);\n"
        "}\n";

    const char* smoothstepFragmentShader =
        "#version 100\n"
        "precision mediump float;\n"
        "varying vec2 v_texCoord;\n"
        "\n"
        "void main() {\n"
        "    vec2 uv = v_texCoord;\n"
        "    float x = uv.x;\n"
        "    float edge0 = 0.3;\n"
        "    float edge1 = 0.7;\n"
        "    float result = smoothstep(edge0, edge1, x);\n"
        "    gl_FragColor = vec4(result, result, result, 1.0);\n"
        "}\n";

    // Fragment shader array for easy access
    const char* fragmentShaders[TEST_COUNT] = {
        absFragmentShader, signFragmentShader, floorFragmentShader, ceilFragmentShader,
        fractFragmentShader, modFragmentShader, minFragmentShader, maxFragmentShader,
        clampFragmentShader, mixFragmentShader, stepFragmentShader, smoothstepFragmentShader
    };

    // Create vertex shader (shared by all programs)
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Create programs for each test
    passedTests = 0;
    for (int i = 0; i < TEST_COUNT; i++) {
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaders[i], NULL);
        glCompileShader(fragmentShader);

        GLuint program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);

        programs[i] = program;

        passedTests++;

        glDeleteShader(fragmentShader);
    }

    glDeleteShader(vertexShader);


    // Initialize OpenGL state
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Create VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
}

void draw() {
    // Clear screen
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render all tests in grid layout
    int windowWidth, windowHeight;
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

    // Calculate cell dimensions with small gaps
    int cellWidth = windowWidth / GRID_COLS;
    int cellHeight = windowHeight / GRID_ROWS;
    int gap = 2; // Small gap between cells

    // Render each test in its grid position
    for (int i = 0; i < TEST_COUNT; i++) {

        // Calculate grid position
        int col = i % GRID_COLS;
        int row = i / GRID_COLS;

        // Calculate viewport position (OpenGL uses bottom-left origin)
        int x = col * cellWidth + gap;
        int y = (GRID_ROWS - 1 - row) * cellHeight + gap;
        int w = cellWidth - 2 * gap;
        int h = cellHeight - 2 * gap;

        // Set viewport for this cell
        glViewport(x, y, w, h);

        // Render the test
        renderTest(i);
    }

    // Swap buffers and poll events
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void renderTest(int testIndex) {

    glUseProgram(programs[testIndex]);

    // Set up vertex attributes
    GLint positionLoc = glGetAttribLocation(programs[testIndex], "a_position");
    GLint texCoordLoc = glGetAttribLocation(programs[testIndex], "a_texCoord");

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    if (positionLoc >= 0) {
        glVertexAttribPointer(positionLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(positionLoc);
    }

    if (texCoordLoc >= 0) {
        glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(texCoordLoc);
    }

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    if (positionLoc >= 0) glDisableVertexAttribArray(positionLoc);
    if (texCoordLoc >= 0) glDisableVertexAttribArray(texCoordLoc);
}
