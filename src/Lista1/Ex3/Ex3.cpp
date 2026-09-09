/*
 * Exercício 3 - Triângulo com cores por vértice
 *   P1 (baixo-esquerda) = vermelho
 *   P2 (baixo-direita)  = verde
 *   P3 (topo)           = azul
 *
 * ─── Resposta 3a) Configuração dos buffers ───────────────────────────────────
 *
 * 1 VBO com dados ENTRELAÇADOS (interleaved): posição xyz + cor rgb por vértice
 *
 * Layout no VBO (cada linha = 1 vértice, 6 floats = 24 bytes):
 *   [ x  y  z  r  g  b ]  ← P1
 *   [ x  y  z  r  g  b ]  ← P2
 *   [ x  y  z  r  g  b ]  ← P3
 *
 * 1 VAO com dois ponteiros de atributo:
 *   • Atributo 0 (posição): 3 floats, stride=6*sizeof(float), offset=0
 *   • Atributo 1 (cor)    : 3 floats, stride=6*sizeof(float), offset=3*sizeof(float)
 *
 * ─── Resposta 3b) Identificação no vertex shader ────────────────────────────
 *
 * layout(location = 0) in vec3 position;  → vinculado ao atributo 0 do VAO
 * layout(location = 1) in vec3 color;     → vinculado ao atributo 1 do VAO
 *
 * A cor é declarada como "out vec3 vColor" no vertex shader e
 * "in vec3 vColor" no fragment shader. A GPU interpola automaticamente
 * o valor entre os vértices (Gouraud shading).
 */

#include <iostream>
using namespace std;
#include <glad/glad.h>
#include <GLFW/glfw3.h>

const GLuint WIDTH = 800, HEIGHT = 600;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

// Vertex shader: recebe posição (loc 0) e cor (loc 1), passa cor interpolada ao frag
const GLchar *vertexShaderSource = R"glsl(
#version 400
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
out vec3 vColor;
void main() {
    gl_Position = vec4(position, 1.0);
    vColor = color;
}
)glsl";

// Fragment shader: usa a cor interpolada vinda do vertex shader
const GLchar *fragmentShaderSource = R"glsl(
#version 400
in vec3 vColor;
out vec4 fragColor;
void main() {
    fragColor = vec4(vColor, 1.0);
}
)glsl";

GLuint setupShader();
GLuint setupGeometry();

int main()
{
    glfwInit();
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT,
        "Ex3 - Triangulo com Cores por Vertice", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);

    GLuint shader = setupShader();
    GLuint VAO    = setupGeometry();

    glUseProgram(shader);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
    return 0;
}

GLuint setupShader()
{
    GLint  success;
    GLchar infoLog[512];

    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vertexShaderSource, NULL);
    glCompileShader(vert);
    glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vert, 512, NULL, infoLog);
        cout << "VERT ERROR: " << infoLog << endl;
    }

    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fragmentShaderSource, NULL);
    glCompileShader(frag);
    glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(frag, 512, NULL, infoLog);
        cout << "FRAG ERROR: " << infoLog << endl;
    }

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
    // Dados entrelaçados: x, y, z, r, g, b  (6 floats por vértice)
    GLfloat vertices[] = {
    //    x       y      z     r     g     b
        -0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // P1 - vermelho
         0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // P2 - verde
         0.0f,   0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  // P3 - azul
    };

    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Atributo 0 → posição: 3 floats, stride=6*float, offset=0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          6 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);

    // Atributo 1 → cor: 3 floats, stride=6*float, offset=3*float
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          6 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return VAO;
}
