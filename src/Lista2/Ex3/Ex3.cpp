/*
 * Lista 2 - Ex3: Cena com Câmera 2D em Pixels
 *
 * Usando ortho(0,800,600,0,-1,1): y=0 no TOPO, y=600 na base.
 * Os objetos são posicionados em pixels — intuitivo para UI e jogos 2D.
 *
 * O que acontece: os vértices são definidos diretamente em pixels,
 * então (400,300) fica exatamente no centro da janela de 800x600.
 *
 * Por que é útil: elimina conversão manual entre coords de tela e NDC.
 * Facilita posicionar HUD, tiles, elementos de interface.
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

// Cena: chão + corpo da casa + telhado + porta
// y=0 = topo da janela (convenção de pixel)
GLuint setupGeometry()
{
    GLfloat vertices[] = {
        // Chão — 2 triângulos (y de 490 a 600 = baixo da tela)
          0.0f, 490.0f, 0.0f,
        800.0f, 490.0f, 0.0f,
        800.0f, 600.0f, 0.0f,
          0.0f, 490.0f, 0.0f,
        800.0f, 600.0f, 0.0f,
          0.0f, 600.0f, 0.0f,

        // Corpo da casa — 2 triângulos (x 260..540, y 310..490)
        260.0f, 310.0f, 0.0f,
        540.0f, 310.0f, 0.0f,
        540.0f, 490.0f, 0.0f,
        260.0f, 310.0f, 0.0f,
        540.0f, 490.0f, 0.0f,
        260.0f, 490.0f, 0.0f,

        // Telhado — 1 triângulo
        210.0f, 310.0f, 0.0f,
        590.0f, 310.0f, 0.0f,
        400.0f, 130.0f, 0.0f,

        // Porta — 2 triângulos (x 360..440, y 395..490)
        360.0f, 395.0f, 0.0f,
        440.0f, 395.0f, 0.0f,
        440.0f, 490.0f, 0.0f,
        360.0f, 395.0f, 0.0f,
        440.0f, 490.0f, 0.0f,
        360.0f, 490.0f, 0.0f,
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
        "Ex3 - Cena em coords de pixel", nullptr, nullptr);
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
    GLint projLoc  = glGetUniformLocation(shader, "projection");

    glUseProgram(shader);
    mat4 proj = ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(proj));

    glClearColor(0.53f, 0.81f, 0.98f, 1.0f); // azul céu
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(VAO);

    glUniform4f(colorLoc, 0.25f, 0.65f, 0.25f, 1.0f); // verde — chão
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glUniform4f(colorLoc, 0.9f, 0.85f, 0.72f, 1.0f);  // bege — corpo
    glDrawArrays(GL_TRIANGLES, 6, 6);

    glUniform4f(colorLoc, 0.72f, 0.18f, 0.12f, 1.0f); // vermelho — telhado
    glDrawArrays(GL_TRIANGLES, 12, 3);

    glUniform4f(colorLoc, 0.5f, 0.3f, 0.1f, 1.0f);    // marrom — porta
    glDrawArrays(GL_TRIANGLES, 15, 6);

    glfwSwapBuffers(window);
    saveFrame("ex3_cena_pixels.png", w, h);

    glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
    return 0;
}
