/*
 * Exercício 4 - Desenho livre com múltiplos VAOs e primitivas diferentes
 *
 * Cena: casa em papel quadriculado (baseada no exemplo da professora)
 *   - Corpo da casa  → GL_TRIANGLES  (2 tri = retângulo cinza claro)
 *   - Telhado        → GL_TRIANGLES  (1 tri = triângulo vermelho escuro)
 *   - Janela (4 panos) → GL_TRIANGLES  (4 retângulos amarelos)
 *   - Contorno janela  → GL_LINE_LOOP
 *   - Cruz janela      → GL_LINES
 *   - Porta          → GL_TRIANGLES  (2 tri = retângulo marrom)
 *   - Contorno porta   → GL_LINE_LOOP
 *   - Contorno casa    → GL_LINE_LOOP  (borda preta)
 *   - Cantos/vértices  → GL_POINTS
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
    float aspect = float(800) / float(600);
    gl_Position = vec4(position.x / aspect, position.y, position.z, 1.0);
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

DrawCall makeRect(float x0, float y0, float x1, float y1, float r, float g, float b)
{
    vector<float> v = {
        x0, y0, 0.0f,   x1, y0, 0.0f,   x1, y1, 0.0f,
        x0, y0, 0.0f,   x1, y1, 0.0f,   x0, y1, 0.0f,
    };
    return { makeVAO(v), 6, GL_TRIANGLES, r, g, b };
}

DrawCall makeLineLoop(vector<float> pts, float r, float g, float b)
{
    return { makeVAO(pts), (int)pts.size() / 3, GL_LINE_LOOP, r, g, b };
}

int main()
{
    glfwInit();
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT,
        "Ex4 - Casa  (ESC sair)", nullptr, nullptr);
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
    glPointSize(10.0f);

    vector<DrawCall> scene;

    // Corpo da casa (cinza claro)
    scene.push_back(makeRect(-0.45f, -0.55f, 0.45f, 0.22f, 0.82f, 0.82f, 0.82f));

    // Telhado (vermelho escuro) — pontas nas laterais da casa, pico um pouco mais alto
    {
        vector<float> v = { -0.45f, 0.22f, 0.0f,
                             0.45f, 0.22f, 0.0f,
                             0.00f, 0.76f, 0.0f };
        scene.push_back({ makeVAO(v), 3, GL_TRIANGLES, 0.75f, 0.12f, 0.12f });
    }

    // Janela — 4 panos amarelos (mais estreita)
    //   TL, TR, BL, BR
    float wx0 = -0.38f, wx1 = -0.25f, wx2 = -0.12f;
    float wy0 = -0.02f, wy1 =  0.07f, wy2 =  0.16f;
    float paneR = 0.78f, paneG = 0.85f, paneB = 0.20f;
    scene.push_back(makeRect(wx0, wy1, wx1, wy2, paneR, paneG, paneB)); // TL
    scene.push_back(makeRect(wx1, wy1, wx2, wy2, paneR, paneG, paneB)); // TR
    scene.push_back(makeRect(wx0, wy0, wx1, wy1, paneR, paneG, paneB)); // BL
    scene.push_back(makeRect(wx1, wy0, wx2, wy1, paneR, paneG, paneB)); // BR

    // Contorno da janela (LINE_LOOP preto)
    scene.push_back(makeLineLoop({
        wx0, wy0, 0.0f,
        wx2, wy0, 0.0f,
        wx2, wy2, 0.0f,
        wx0, wy2, 0.0f,
    }, 0.0f, 0.0f, 0.0f));

    // Cruz da janela (LINES preto)
    {
        vector<float> v = {
            wx1, wy0, 0.0f,   wx1, wy2, 0.0f,  // vertical
            wx0, wy1, 0.0f,   wx2, wy1, 0.0f,  // horizontal
        };
        scene.push_back({ makeVAO(v), 4, GL_LINES, 0.0f, 0.0f, 0.0f });
    }

    // Porta (marrom) — centralizada e um pouco mais baixa
    scene.push_back(makeRect(-0.12f, -0.55f, 0.12f, -0.08f, 0.45f, 0.25f, 0.10f));

    // Contorno da porta (LINE_LOOP preto)
    scene.push_back(makeLineLoop({
        -0.12f, -0.55f, 0.0f,
         0.12f, -0.55f, 0.0f,
         0.12f, -0.08f, 0.0f,
        -0.12f, -0.08f, 0.0f,
    }, 0.0f, 0.0f, 0.0f));

    // Contorno do corpo da casa (LINE_LOOP preto)
    scene.push_back(makeLineLoop({
        -0.45f, -0.55f, 0.0f,
         0.45f, -0.55f, 0.0f,
         0.45f,  0.22f, 0.0f,
        -0.45f,  0.22f, 0.0f,
    }, 0.0f, 0.0f, 0.0f));

    // Pontos nos cantos (POINTS preto)
    {
        vector<float> v = {
            -0.45f, -0.55f, 0.0f,
             0.45f, -0.55f, 0.0f,
             0.45f,  0.22f, 0.0f,
            -0.45f,  0.22f, 0.0f,
             0.00f,  0.76f, 0.0f,  // pico do telhado
        };
        scene.push_back({ makeVAO(v), 5, GL_POINTS, 0.0f, 0.0f, 0.0f });
    }

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClearColor(0.20f, 0.20f, 0.20f, 1.0f); // fundo cinza escuro (papel quadriculado)
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
