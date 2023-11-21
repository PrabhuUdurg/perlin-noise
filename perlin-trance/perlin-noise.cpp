#define GLFW_INCLUDE_NONE
#define GL_SILENCE_DEPRECATION
#include <iostream>
#include <algorithm>
#include <GLFW/glfw3.h>
#include "glad/include/glad/glad.h"


GLuint VBO, VAO;
const char *vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in float aPos;
    void main() {
        gl_Position = vec4(aPos, 0.0, 0.0, 1.0);
    }
)";

const char *fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    void main() {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
)";

int createWindow(int length, unsigned long width, float *fOutput, int nCount)
{
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, length, "window", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Set up vertex data (VBO)
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, nCount * sizeof(float), fOutput, GL_STATIC_DRAW);

    // Set up vertex array object (VAO)
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Create and compile shaders
    unsigned int vertexShader, fragmentShader, shaderProgram;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Check for shader compilation errors
    int success;
    char infoLog[512];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "Vertex shader compilation failed\n" << infoLog << std::endl;
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "Fragment shader compilation failed\n" << infoLog << std::endl;
    }

    // Create shader program and link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for program linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Shader program linking failed\n" << infoLog << std::endl;
    }

    // Delete shaders after linking
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        // Render here
        glClear(GL_COLOR_BUFFER_BIT);

        // Use the shader program and VAO, draw the data
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINE_STRIP, 0, nCount);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}


// Generate Perlin Noise
void PerlinNoise1D(int nCount, float const *fSeed, int nOctaves, float *fOutput)
{
    // Loop to go through random noise
    for (int x = 0; x < nCount; x++)
    {
        // Loop through noise octaves
        float fNoise = 0.0f;
        float fScale = 1.0f;
        float fScaleAcc = 0.0f;

        for (int o = 0; o < nOctaves; o++)
        {
            int nPitch = nCount >> o;
            int nSample1 = (x / nPitch) * nPitch;
            int nSample2 = (nSample1 + nPitch) % nCount;

            float fBlend = (float)(x - nSample1) / (float)nPitch;
            // Linear Interpolation
            float fSample = (1.0f - fBlend) * fSeed[nSample1] + fBlend * fSeed[nSample2];
            fNoise += fSample * fScale;
            fScaleAcc += fScale;
            fScale = fScale / 2.0f;
        }

        // Scale to seed range
        fOutput[x] = fNoise / fScale;
    }
}

int main()
{
    const int nCount = 800; // Adjust the size as needed
    float fSeed[nCount];
    int nOctaves = 4; // Adjust the number of octaves as needed
    float fOutput[nCount];

    // Initialize fSeed with random values
    for (float & i : fSeed)
    {
        i = static_cast<float>(rand()) / RAND_MAX;
    }

    PerlinNoise1D(nCount, fSeed, nOctaves, fOutput);
    createWindow(500, 500, fOutput, nCount);
}