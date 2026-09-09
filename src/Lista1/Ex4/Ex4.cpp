/*
 * Exercício 4 - Desenho livre com múltiplos VAOs e primitivas diferentes
 *
 * Cena: casa simples com sol
 *   - Chão         → GL_TRIANGLES  (2 tri = retângulo verde)
 *   - Corpo casa   → GL_TRIANGLES  (2 tri = retângulo branco)
 *   - Telhado      → GL_TRIANGLES  (1 tri = triângulo vermelho)
 *   - Porta        → GL_TRIANGLES  (2 tri = retângulo marrom)
 *   - Janela       → GL_LINE_LOOP  (quadrado azul sem fill)
 *   - Cruz janela  → GL_LINES      (2 linhas)
 *   - Sol          → GL_TRIANGLE_FAN (círculo amarelo)
 *
 * Cada forma tem seu próprio VAO e sua própria chamada de desenho (drawcall).
 */

#include <iostream>
#include <vector>
#include <cmath>
using namespace std;
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const GLuint WIDTH = 800, HEIGHT = 600;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
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

struct DrawCall {
    GLuint VAO;
    int    nVerts;
    GLenum primitive;
    float  r, g, b;
};

GLuint setupShader();

GLuint makeVAO(const vector<float> &v)
{
    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), v.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid *)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return VAO;
}

// Monta DrawCall para um retangulo definido por (x0,y0)-(x1,y1)
DrawCall makeRect(float x0, float y0, float x1, float y1, float r, float g, float b)
{
    vector<float> v = {
        x0, y0, 0.0f,   x1, y0, 0.0f,   x1, y1, 0.0f,
        x0, y0, 0.0f,   x1, y1, 0.0f,   x0, y1, 0.0f,
    };
    return { makeVAO(v), 6, GL_TRIANGLES, r, g, b };
}

int main()
{
    glfwInit();
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT,
        "Ex4 - Cena com multiplos VAOs  (ESC sair)", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);

    GLuint shader   = setupShader();
    GLint  colorLoc = glGetUniformLocation(shader, "inputColor");
    glUseProgram(shader);
    glLineWidth(2.0f);

    vector<DrawCall> scene;

    // Chão (retângulo verde)
    scene.push_back(makeRect(-1.0f, -1.0f, 1.0f, -0.35f, 0.2f, 0.55f, 0.2f));

    // Corpo da casa (retângulo branco)
    scene.push_back(makeRect(-0.45f, -0.35f, 0.45f, 0.2f, 0.9f, 0.9f, 0.85f));

    // Telhado (triângulo vermelho-escuro)
    {
        vector<float> v = { -0.55f, 0.2f, 0.0f,   0.55f, 0.2f, 0.0f,   0.0f, 0.65f, 0.0f };
        scene.push_back({ makeVAO(v), 3, GL_TRIANGLES, 0.7f, 0.2f, 0.1f });
    }

    // Porta (retângulo marrom)
    scene.push_back(makeRect(-0.10f, -0.35f, 0.10f, 0.05f, 0.5f, 0.3f, 0.1f));

    // Janela - contorno (GL_LINE_LOOP)
    {
        vector<float> v = {
            -0.38f, 0.00f, 0.0f,
            -0.18f, 0.00f, 0.0f,
            -0.18f, 0.18f, 0.0f,
            -0.38f, 0.18f, 0.0f,
        };
        scene.push_back({ makeVAO(v), 4, GL_LINE_LOOP, 0.3f, 0.5f, 0.9f });
    }

    // Cruz da janela (GL_LINES: 2 segmentos = 4 vértices)
    {
        vector<float> v = {
            -0.28f, 0.00f, 0.0f,   -0.28f, 0.18f, 0.0f,  // vertical
            -0.38f, 0.09f, 0.0f,   -0.18f, 0.09f, 0.0f,  // horizontal
        };
        scene.push_back({ makeVAO(v), 4, GL_LINES, 0.3f, 0.5f, 0.9f });
    }

    // Sol (círculo amarelo, GL_TRIANGLE_FAN)
    {
        vector<float> v;
        float cx = 0.72f, cy = 0.72f, r = 0.16f;
        int   n  = 48;
        v.push_back(cx); v.push_back(cy); v.push_back(0.0f);
        for (int i = 0; i <= n; i++)
        {
            float a = 2.0f * (float)M_PI * i / n;
            v.push_back(cx + r * cosf(a));
            v.push_back(cy + r * sinf(a));
            v.push_back(0.0f);
        }
        scene.push_back({ makeVAO(v), (int)v.size() / 3, GL_TRIANGLE_FAN, 1.0f, 0.9f, 0.1f });
    }

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClearColor(0.5f, 0.75f, 0.95f, 1.0f); // céu azul como fundo
        glClear(GL_COLOR_BUFFER_BIT);

        for (auto &dc : scene)
        {
            glBindVertexArray(dc.VAO);
            glUniform4f(colorLoc, dc.r, dc.g, dc.b, 1.0f);
            glDrawArrays(dc.primitive, 0, dc.nVerts);
        }

        glfwSwapBuffers(window);
    }

    for (auto &dc : scene)
        glDeleteVertexArrays(1, &dc.VAO);
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
