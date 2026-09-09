/*
 * Exercício 1 - Dois triângulos com diferentes modos de desenho
 *
 * Teclas:
 *   1 - a) Apenas preenchido (GL_FILL)
 *   2 - b) Apenas contorno  (GL_LINE)
 *   3 - c) Apenas pontos    (GL_POINT)
 *   4 - d) Os três modos juntos
 *   ESC - fechar
 */

#include <iostream>
using namespace std;
#include <glad/glad.h>
#include <GLFW/glfw3.h>

const GLuint WIDTH = 800, HEIGHT = 600;

// 0=fill, 1=wire, 2=points, 3=all
int drawMode = 0;

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

int main()
{
    glfwInit();
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

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);

        switch (drawMode)
        {
        case 0: // a) Apenas preenchido
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glUniform4f(colorLoc, 0.2f, 0.5f, 0.9f, 1.0f);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            break;

        case 1: // b) Apenas contorno
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;

        case 2: // c) Apenas pontos
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            glUniform4f(colorLoc, 1.0f, 0.5f, 0.0f, 1.0f);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;

        case 3: // d) Os tres modos juntos
            // 1) Preenchido
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glUniform4f(colorLoc, 0.2f, 0.5f, 0.9f, 1.0f);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // 2) Contorno sobre o fill
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // 3) Pontos sobre tudo
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            glUniform4f(colorLoc, 1.0f, 0.3f, 0.0f, 1.0f);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        }

        glfwSwapBuffers(window);
    }

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
    // Dois triangulos lado a lado
    GLfloat vertices[] = {
        // Triangulo 0 (esquerda)
        -0.75f, -0.5f, 0.0f,
        -0.05f, -0.5f, 0.0f,
        -0.40f,  0.5f, 0.0f,
        // Triangulo 1 (direita)
         0.05f, -0.5f, 0.0f,
         0.75f, -0.5f, 0.0f,
         0.40f,  0.5f, 0.0f,
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
