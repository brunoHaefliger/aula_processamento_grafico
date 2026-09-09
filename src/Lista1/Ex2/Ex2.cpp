/*
 * Exercício 2 - Círculo e formas com equação paramétrica
 *
 * Teclas:
 *   1 - Círculo        (N=64 segmentos)
 *   2 - Octágono       (N=8)
 *   3 - Pentágono      (N=5)
 *   4 - Pac-Man        (círculo com boca)
 *   5 - Fatia de pizza (setor circular)
 *   6 - Estrela        (DESAFIO 1)
 *   7 - Espiral        (DESAFIO 2)
 *   ESC - fechar
 *
 * Princípio: equação paramétrica do círculo
 *   x = cx + r * cos(theta)
 *   y = cy + r * sin(theta)
 * Variando theta de 0 a 2*PI gera um círculo.
 * Com menos pontos (theta = 2*PI/N por passo) gera polígonos regulares.
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

enum Shape { CIRCLE, OCTAGON, PENTAGON, PACMAN, PIZZA, STAR, SPIRAL, SHAPE_COUNT };
int currentShape = CIRCLE;

const char *shapeNames[] = {
    "Circulo [1]", "Octagono [2]", "Pentagono [3]",
    "Pac-Man [4]", "Fatia de Pizza [5]", "Estrela [6]", "Espiral [7]"
};

struct ShapeData {
    GLuint VAO;
    int    nVertices;
    GLenum primitive;
};
ShapeData shapes[SHAPE_COUNT];

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

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_1) currentShape = CIRCLE;
        if (key == GLFW_KEY_2) currentShape = OCTAGON;
        if (key == GLFW_KEY_3) currentShape = PENTAGON;
        if (key == GLFW_KEY_4) currentShape = PACMAN;
        if (key == GLFW_KEY_5) currentShape = PIZZA;
        if (key == GLFW_KEY_6) currentShape = STAR;
        if (key == GLFW_KEY_7) currentShape = SPIRAL;
    }
}

GLuint setupShader();

// Envia um vetor de floats (xyz) para um VAO/VBO
GLuint uploadVertices(const vector<float> &v)
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

// Círculo / N-gon: GL_TRIANGLE_FAN com vértice central + N pontos no perímetro
ShapeData makeNGon(float cx, float cy, float r, int n)
{
    vector<float> v;
    v.push_back(cx); v.push_back(cy); v.push_back(0.0f); // centro (pivo do fan)
    for (int i = 0; i <= n; i++) // i==n repete o primeiro ponto para fechar
    {
        float a = 2.0f * (float)M_PI * i / n;
        v.push_back(cx + r * cosf(a));
        v.push_back(cy + r * sinf(a));
        v.push_back(0.0f);
    }
    ShapeData sd;
    sd.VAO       = uploadVertices(v);
    sd.nVertices = (int)v.size() / 3; // n+2
    sd.primitive = GL_TRIANGLE_FAN;
    return sd;
}

// Pac-Man: arco de mouthAngle até 2*PI - mouthAngle (boca aberta à direita)
ShapeData makePacman(float cx, float cy, float r, int n, float mouthAngle)
{
    vector<float> v;
    v.push_back(cx); v.push_back(cy); v.push_back(0.0f);
    float startA = mouthAngle;
    float endA   = 2.0f * (float)M_PI - mouthAngle;
    int   nSeg   = (int)((endA - startA) / (2.0f * (float)M_PI) * n);
    for (int i = 0; i <= nSeg; i++)
    {
        float a = startA + (endA - startA) * i / nSeg;
        v.push_back(cx + r * cosf(a));
        v.push_back(cy + r * sinf(a));
        v.push_back(0.0f);
    }
    ShapeData sd;
    sd.VAO       = uploadVertices(v);
    sd.nVertices = (int)v.size() / 3;
    sd.primitive = GL_TRIANGLE_FAN;
    return sd;
}

// Fatia de pizza: setor de startAngle até startAngle+sectorAngle
ShapeData makePizzaSlice(float cx, float cy, float r, float startAngle, float sectorAngle, int n)
{
    vector<float> v;
    v.push_back(cx); v.push_back(cy); v.push_back(0.0f);
    for (int i = 0; i <= n; i++)
    {
        float a = startAngle + sectorAngle * i / n;
        v.push_back(cx + r * cosf(a));
        v.push_back(cy + r * sinf(a));
        v.push_back(0.0f);
    }
    ShapeData sd;
    sd.VAO       = uploadVertices(v);
    sd.nVertices = (int)v.size() / 3;
    sd.primitive = GL_TRIANGLE_FAN;
    return sd;
}

// Estrela de nPoints pontas: alterna raio externo/interno com GL_TRIANGLE_FAN
ShapeData makeStar(float cx, float cy, float rOuter, float rInner, int nPoints)
{
    vector<float> v;
    v.push_back(cx); v.push_back(cy); v.push_back(0.0f);
    for (int i = 0; i <= 2 * nPoints; i++)
    {
        float r = (i % 2 == 0) ? rOuter : rInner;
        // começa no topo (-PI/2) e gira em sentido horário
        float a = -(float)M_PI / 2.0f + (float)i * (float)M_PI / nPoints;
        v.push_back(cx + r * cosf(a));
        v.push_back(cy + r * sinf(a));
        v.push_back(0.0f);
    }
    ShapeData sd;
    sd.VAO       = uploadVertices(v);
    sd.nVertices = (int)v.size() / 3;
    sd.primitive = GL_TRIANGLE_FAN;
    return sd;
}

// Espiral: GL_LINE_STRIP onde o raio cresce com o ângulo
ShapeData makeSpiral(float cx, float cy, float rMin, float rMax, int nLoops, int nSeg)
{
    vector<float> v;
    for (int i = 0; i <= nSeg; i++)
    {
        float t = (float)i / nSeg;
        float a = t * 2.0f * (float)M_PI * nLoops;
        float r = rMin + (rMax - rMin) * t;
        v.push_back(cx + r * cosf(a));
        v.push_back(cy + r * sinf(a));
        v.push_back(0.0f);
    }
    ShapeData sd;
    sd.VAO       = uploadVertices(v);
    sd.nVertices = (int)v.size() / 3;
    sd.primitive = GL_LINE_STRIP;
    return sd;
}

int main()
{
    glfwInit();
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Ex2", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);

    GLuint shader   = setupShader();
    GLint colorLoc  = glGetUniformLocation(shader, "inputColor");
    glUseProgram(shader);
    glLineWidth(2.0f);

    // Gera todas as formas uma vez na inicialização
    shapes[CIRCLE]  = makeNGon(0, 0, 0.7f, 64);
    shapes[OCTAGON] = makeNGon(0, 0, 0.7f, 8);
    shapes[PENTAGON]= makeNGon(0, 0, 0.7f, 5);
    // boca de 30 graus para cada lado = 60 graus no total
    shapes[PACMAN]  = makePacman(0, 0, 0.7f, 64, (float)(M_PI / 6.0));
    // setor de 90 graus (1/4 de pizza)
    shapes[PIZZA]   = makePizzaSlice(0, 0, 0.7f, -(float)(M_PI / 4.0), (float)(M_PI / 2.0), 32);
    // estrela de 5 pontas
    shapes[STAR]    = makeStar(0, 0, 0.7f, 0.28f, 5);
    // espiral com 3 voltas
    shapes[SPIRAL]  = makeSpiral(0, 0, 0.05f, 0.7f, 3, 300);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        char title[128];
        sprintf(title, "Ex2 - Forma atual: %s  (teclas 1-7)", shapeNames[currentShape]);
        glfwSetWindowTitle(window, title);

        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(shapes[currentShape].VAO);
        glUniform4f(colorLoc, 0.9f, 0.75f, 0.1f, 1.0f); // amarelo-dourado
        glDrawArrays(shapes[currentShape].primitive, 0, shapes[currentShape].nVertices);

        glfwSwapBuffers(window);
    }

    for (int i = 0; i < SHAPE_COUNT; i++)
        glDeleteVertexArrays(1, &shapes[i].VAO);
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
