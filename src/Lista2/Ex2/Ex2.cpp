/*
 * Lista 2 - Ex2: Projeção Ortogonal com Coordenadas de Pixel
 *
 * Modifica para: xmin=0, xmax=800, ymin=600, ymax=0
 * y=0 fica no TOPO, y=600 na base — convenção de tela (útil para UI/2D).
 * O triângulo agora é definido diretamente em pixels.
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

GLuint setupGeometry()
{
    // Vértices em pixels (800x600, y=0 no topo)
    // Triângulo apontando para cima: base embaixo, apex no topo
    GLfloat vertices[] = {
        150.0f, 440.0f, 0.0f,  // v0 base esquerda
        650.0f, 440.0f, 0.0f,  // v1 base direita
        400.0f,  80.0f, 0.0f,  // v2 apex (topo)
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
        "Ex2 - Ortho: coords de pixel", nullptr, nullptr);
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

    // Coordenadas de pixel: x=0 na esquerda, y=0 no TOPO
    mat4 proj = ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(proj));

    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(VAO);
    glUniform4f(colorLoc, 1.0f, 0.85f, 0.0f, 1.0f); // amarelo
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glfwSwapBuffers(window);
    saveFrame("ex2_ortho_pixel.png", w, h);

    glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
    return 0;
}
