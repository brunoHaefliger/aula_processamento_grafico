/*
 * Exercício 1 - Dois triângulos com diferentes modos de desenho
 *
 * Teclas:
 *   1 - a) Apenas preenchido (GL_FILL)
 *   2 - b) Apenas contorno  (GL_LINE)
 *   3 - c) Apenas pontos    (GL_POINT)
 *   4 - d) Os três modos juntos
 *   ESC - fechar
 *
 * Modo PNG (definir SAVE_PNG abaixo):
 *   Renderiza os 4 modos em arquivos PNG e encerra sem abrir janela.
 */

#define SAVE_PNG  

#include <iostream>
#include <vector>
using namespace std;
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

const GLuint WIDTH = 800, HEIGHT = 600;

// 0=fill, 1=wire, 2=points, 3=all
int drawMode = 0;

void saveFrame(const char *filename, int w, int h)
{
    vector<unsigned char> pixels(w * h * 3);
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    // OpenGL tem Y=0 embaixo; PNG tem Y=0 em cima — inverte
    for (int y = 0; y < h / 2; y++)
        swap_ranges(pixels.begin() + y * w * 3,
                    pixels.begin() + (y + 1) * w * 3,
                    pixels.begin() + (h - y - 1) * w * 3);
    stbi_write_png(filename, w, h, 3, pixels.data(), w * 3);
    cout << "Salvo: " << filename << endl;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_1) drawMode = 0;
        if (key == GLFW_KEY_2) drawMode = 1;
        if (key == GLFW_KEY_3) drawMode = 2;
        if (key == GLFW_KEY_4) drawMode = 3;
    }
}

const GLchar *vertexShaderSource = R"glsl(
#version 400
layout (location = 0) in vec3 position;
void main() {
    gl_Position = vec4(position, 1.0);
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

GLuint setupShader();
GLuint setupGeometry();

void renderMode(GLuint VAO, GLint colorLoc, int mode)
{
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(VAO);

    switch (mode)
    {
    case 0:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glUniform4f(colorLoc, 0.2f, 0.5f, 0.9f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        break;
    case 1:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    case 2:
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        glUniform4f(colorLoc, 1.0f, 0.5f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    case 3:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glUniform4f(colorLoc, 0.2f, 0.5f, 0.9f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        glUniform4f(colorLoc, 1.0f, 0.3f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    }
}

int main()
{
    glfwInit();

#ifdef SAVE_PNG
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // janela oculta
#endif

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT,
        "Ex1 - [1] Fill  [2] Contorno  [3] Pontos  [4] Todos", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);

    GLuint shader = setupShader();
    GLuint VAO    = setupGeometry();
    GLint colorLoc = glGetUniformLocation(shader, "inputColor");

    glUseProgram(shader);
    glLineWidth(2.0f);
    glPointSize(12.0f);

#ifdef SAVE_PNG
    const char *names[] = { "ex1_a_fill.png", "ex1_b_contorno.png",
                             "ex1_c_pontos.png", "ex1_d_todos.png" };
    for (int i = 0; i < 4; i++)
    {
        renderMode(VAO, colorLoc, i);
        glfwSwapBuffers(window);
        saveFrame(names[i], w, h);
    }
#else
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        renderMode(VAO, colorLoc, drawMode);
        glfwSwapBuffers(window);
    }
#endif

    glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
    return 0;
}

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

GLuint setupGeometry()
{
    GLfloat vertices[] = {
        // Triangulo 0 (esquerda) — v0, v1, v2
        -0.5f,  0.5f, 0.0f,   // v0
        -0.5f, -0.5f, 0.0f,   // v1
         0.0f,  0.0f, 0.0f,   // v2
        // Triangulo 1 (direita) — v3, v4, v5
         0.0f,  0.0f, 0.0f,   // v3
         0.5f, -0.5f, 0.0f,   // v4
         0.5f,  0.5f, 0.0f,   // v5
    };

    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Atributo 0: posicao xyz, stride=3 floats, offset=0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return VAO;
}
