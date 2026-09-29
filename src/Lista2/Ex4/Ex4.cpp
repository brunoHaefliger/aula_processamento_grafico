/*
 * Lista 2 - Ex4: Viewport Restrito ao Quadrante Superior Direito
 *
 * Passo 1: viewport completo -> fundo + grade + borda vermelha.
 * Passo 2: glViewport(400, 300, 400, 300) -> quadrante superior direito.
 * Passo 3: triângulo magenta renderizado apenas nesse quadrante.
 */

#include <iostream>
#include <vector>
using namespace std;
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

const GLuint WIDTH = 800, HEIGHT = 600;

void saveFrame(const char *filename, int w, int h)
{
    vector<unsigned char> pixels(w * h * 3);
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    for (int y = 0; y < h / 2; y++)
        swap_ranges(pixels.begin() + y * w * 3,
                    pixels.begin() + (y + 1) * w * 3,
                    pixels.begin() + (h - y - 1) * w * 3);
    stbi_write_png(filename, w, h, 3, pixels.data(), w * 3);
    printf("Salvo: %s\n", filename);
}

void key_callback(GLFWwindow *window, int key, int, int action, int)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

const GLchar *vertexShaderSource = R"glsl(
#version 400
layout (location = 0) in vec3 position;
uniform mat4 projection;
void main() {
    gl_Position = projection * vec4(position, 1.0);
}
)glsl";

const GLchar *fragmentShaderSource = R"glsl(
#version 400
uniform vec4 inputColor;
out vec4 color;
void main() {
    color = inputColor;
}
)glsl";

GLuint setupShader()
{
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vertexShaderSource, NULL);
    glCompileShader(vert);
    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fragmentShaderSource, NULL);
    glCompileShader(frag);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);
    glDeleteShader(vert);
    glDeleteShader(frag);
    return prog;
}

// grade e borda vermelha (coords de pixel)
GLuint setupLines(GLuint &countLines, GLuint &countBorder)
{
    GLfloat vertices[] = {
        // grade
          0.0f, 300.0f, 0.0f,
        800.0f, 300.0f, 0.0f,
        400.0f,   0.0f, 0.0f,
        400.0f, 600.0f, 0.0f,

        // borda vermelha: LINE_LOOP do quadrante sup-dir
        403.0f,   4.0f, 0.0f,
        797.0f,   4.0f, 0.0f,
        797.0f, 296.0f, 0.0f,
        403.0f, 296.0f, 0.0f,
    };
    countLines  = 4;
    countBorder = 4;

    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return VAO;
}

// triângulo em coords do mundo
GLuint setupTriangle()
{
    GLfloat vertices[] = {
        -7.0f, -6.0f, 0.0f,
         7.0f, -6.0f, 0.0f,
         0.0f,  7.5f, 0.0f,
    };
    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return VAO;
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT,
        "Ex4 - Viewport: quadrante sup-dir", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    int w, h;
    glfwGetFramebufferSize(window, &w, &h);

    GLuint shader     = setupShader();
    GLuint cntLines = 0, cntBorder = 0;
    GLuint VAO_lines  = setupLines(cntLines, cntBorder);
    GLuint VAO_tri    = setupTriangle();
    GLint colorLoc    = glGetUniformLocation(shader, "inputColor");
    GLint projLoc     = glGetUniformLocation(shader, "projection");

    glUseProgram(shader);
    glLineWidth(2.0f);

    // Passo 1: viewport completo - fundo + grade + borda
    glViewport(0, 0, w, h);

    mat4 projPixel = ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(projPixel));

    glClearColor(0.27f, 0.51f, 0.71f, 1.0f); // azul-aço
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(VAO_lines);

    // Grade — cinza claro
    glUniform4f(colorLoc, 0.8f, 0.8f, 0.8f, 1.0f);
    glDrawArrays(GL_LINES, 0, cntLines);

    // Borda vermelha do quadrante sup-dir
    glUniform4f(colorLoc, 1.0f, 0.1f, 0.1f, 1.0f);
    glLineWidth(3.0f);
    glDrawArrays(GL_LINE_LOOP, cntLines, cntBorder);
    glLineWidth(2.0f);

    // Passo 2: viewport restrito - quadrante superior direito
    // y=0 na base (OpenGL) -> y=300 é a metade de cima
    glViewport(400, 300, 400, 300);

    mat4 projMundo = ortho(-10.0f, 10.0f, -10.0f, 10.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(projMundo));

    glBindVertexArray(VAO_tri);
    glUniform4f(colorLoc, 1.0f, 0.0f, 1.0f, 1.0f); // magenta
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
    saveFrame("ex4_viewport_quadrante.png", w, h);

    glDeleteVertexArrays(1, &VAO_lines);
    glDeleteVertexArrays(1, &VAO_tri);
    glfwTerminate();
    return 0;
}
