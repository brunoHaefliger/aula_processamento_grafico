/*
 * Lista 2 - Ex4b: Mesma Cena nos 4 Quadrantes
 *
 * Define glViewport para cada quadrante e repete o desenho.
 * O espaço NDC [-1,1]² é mapeado para cada área individualmente.
 *
 * Quadrantes (OpenGL: y=0 na BASE):
 *   Sup-esq : glViewport(  0, 300, 400, 300)
 *   Sup-dir : glViewport(400, 300, 400, 300)
 *   Inf-esq : glViewport(  0,   0, 400, 300)
 *   Inf-dir : glViewport(400,   0, 400, 300)
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

// Linhas da grade (coords de pixel, y=0 no topo)
GLuint setupGrid()
{
    GLfloat vertices[] = {
          0.0f, 300.0f, 0.0f,  800.0f, 300.0f, 0.0f,  // horizontal
        400.0f,   0.0f, 0.0f,  400.0f, 600.0f, 0.0f,  // vertical
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
        "Ex4b - 4 Quadrantes", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    int w, h;
    glfwGetFramebufferSize(window, &w, &h);

    GLuint shader   = setupShader();
    GLuint VAO_tri  = setupTriangle();
    GLuint VAO_grid = setupGrid();
    GLint colorLoc  = glGetUniformLocation(shader, "inputColor");
    GLint projLoc   = glGetUniformLocation(shader, "projection");

    glUseProgram(shader);
    glLineWidth(2.0f);

    // ── Fundo + grade com viewport completo ─────────────────────────────
    glViewport(0, 0, w, h);

    mat4 projPixel = ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(projPixel));

    glClearColor(0.27f, 0.51f, 0.71f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(VAO_grid);
    glUniform4f(colorLoc, 0.8f, 0.8f, 0.8f, 1.0f);
    glDrawArrays(GL_LINES, 0, 4);

    // ── Triângulo em cada um dos 4 quadrantes ────────────────────────────
    mat4 projMundo = ortho(-10.0f, 10.0f, -10.0f, 10.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(projMundo));
    glUniform4f(colorLoc, 1.0f, 0.0f, 1.0f, 1.0f); // magenta

    struct { int x, y; } quadrantes[] = {
        {  0, 300 },  // superior esquerdo
        { 400, 300 }, // superior direito
        {  0,   0 },  // inferior esquerdo
        { 400,   0 }, // inferior direito
    };

    glBindVertexArray(VAO_tri);
    for (auto &q : quadrantes)
    {
        glViewport(q.x, q.y, 400, 300);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    glfwSwapBuffers(window);
    saveFrame("ex4b_4quadrantes.png", w, h);

    glDeleteVertexArrays(1, &VAO_tri);
    glDeleteVertexArrays(1, &VAO_grid);
    glfwTerminate();
    return 0;
}
