/*
 * Lista 2 - Ex6: Triângulos por Clique do Mouse
 *
 * Clique esquerdo -> adiciona 1 vértice.
 * A cada 3 vértices -> completa 1 triângulo com nova cor.
 * Tecla C -> limpa tudo.  ESC -> fechar.
 *
 * Câmera: ortho(0,800,600,0,-1,1) — coords de pixel, y=0 no topo.
 * Mouse GLFW já está nesse espaço: sem conversão necessária.
 */

#include <iostream>
#include <vector>
#include <string>
using namespace std;
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

const GLuint WIDTH = 800, HEIGHT = 600;
const int    MAX_VERTS = 999; // 333 triângulos máximo

vector<vec2> g_verts;
bool         g_dirty = false;
GLuint       g_VAO, g_VBO;

const vec3 PALETTE[] = {
    {0.9f, 0.15f, 0.15f}, // vermelho
    {0.2f, 0.80f, 0.25f}, // verde
    {0.2f, 0.50f, 1.00f}, // azul
    {1.0f, 0.80f, 0.00f}, // amarelo
    {1.0f, 0.40f, 0.00f}, // laranja
    {0.9f, 0.10f, 0.90f}, // magenta
    {0.0f, 0.80f, 0.80f}, // ciano
    {0.6f, 0.80f, 0.20f}, // limão
};
const int N_COLORS = (int)(sizeof(PALETTE) / sizeof(PALETTE[0]));

void mouse_button_callback(GLFWwindow *window, int button, int action, int)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if ((int)g_verts.size() >= MAX_VERTS) return;
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        g_verts.push_back({(float)x, (float)y});
        g_dirty = true;
        int n = (int)g_verts.size();
        if (n % 3 == 0)
            printf("Triângulo %d criado!  (total de vértices: %d)\n", n / 3, n);
        else
            printf("Vértice %d adicionado  (%d/3 do próximo triângulo)\n",
                   n, n % 3);
    }
}

void key_callback(GLFWwindow *window, int key, int, int action, int)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        g_verts.clear();
        g_dirty = true;
        puts("Limpou todos os vértices.");
    }
}

const GLchar *vertSrc = R"glsl(
#version 400
layout (location = 0) in vec2 position;
uniform mat4 projection;
void main() {
    gl_Position = projection * vec4(position, 0.0, 1.0);
}
)glsl";

const GLchar *fragSrc = R"glsl(
#version 400
uniform vec4 inputColor;
out vec4 color;
void main() {
    color = inputColor;
}
)glsl";

GLuint buildShader()
{
    GLuint v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, 1, &vertSrc, NULL); glCompileShader(v);
    GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f, 1, &fragSrc, NULL); glCompileShader(f);
    GLuint p = glCreateProgram();
    glAttachShader(p, v); glAttachShader(p, f);
    glLinkProgram(p);
    glDeleteShader(v); glDeleteShader(f);
    return p;
}

void setupBuffers()
{
    glGenVertexArrays(1, &g_VAO);
    glGenBuffers(1, &g_VBO);
    glBindVertexArray(g_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
    // Pré-aloca buffer para MAX_VERTS vec2
    glBufferData(GL_ARRAY_BUFFER, MAX_VERTS * sizeof(vec2), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (GLvoid *)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

int main()
{
    glfwInit();
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT,
        "Ex6 — Clique para criar vértices  |  C = limpar  |  ESC = sair",
        nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);
    glPointSize(8.0f);

    GLuint shader  = buildShader();
    GLint colorLoc = glGetUniformLocation(shader, "inputColor");
    GLint projLoc  = glGetUniformLocation(shader, "projection");
    setupBuffers();

    glUseProgram(shader);
    mat4 proj = ortho(0.0f, (float)w, (float)h, 0.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(proj));

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Atualiza VBO se houve mudança
        if (g_dirty && !g_verts.empty()) {
            glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0,
                            g_verts.size() * sizeof(vec2), g_verts.data());
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            g_dirty = false;

            // Atualiza título com contagem
            string title = "Ex6  |  Vértices: " + to_string(g_verts.size()) +
                           "  |  Triângulos: " + to_string(g_verts.size() / 3) +
                           "  |  C=limpar  ESC=sair";
            glfwSetWindowTitle(window, title.c_str());
        }

        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (!g_verts.empty()) {
            glBindVertexArray(g_VAO);

            int n_complete  = ((int)g_verts.size() / 3) * 3;
            int n_remaining = (int)g_verts.size() - n_complete;

            // Triângulos completos — cada um com sua cor
            for (int i = 0; i < n_complete; i += 3) {
                const vec3 &c = PALETTE[(i / 3) % N_COLORS];
                glUniform4f(colorLoc, c.r, c.g, c.b, 1.0f);
                glDrawArrays(GL_TRIANGLES, i, 3);
            }

            // Pontos aguardando o próximo triângulo — branco
            if (n_remaining > 0) {
                glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
                glDrawArrays(GL_POINTS, n_complete, n_remaining);
            }
        }

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &g_VAO);
    glDeleteBuffers(1, &g_VBO);
    glfwTerminate();
    return 0;
}
