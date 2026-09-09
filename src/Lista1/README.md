# Lista de Exercícios 1 – Primitivas Gráficas, Shaders & Buffers

## Equipe
- Bruno Haefliger

---

## Descrição do Projeto

Exercícios desenvolvidos como parte da disciplina *Processamento Gráfico: Fundamentos* com o objetivo de aplicar conceitos de OpenGL moderna e C++ para desenho de primitivas gráficas utilizando shaders e buffers (VBO/VAO).

---

## Estrutura do Projeto

| Arquivo | Descrição |
|---|---|
| `Ex1/Ex1.cpp` | Dois triângulos com modos fill, contorno e pontos (teclas 1–4) |
| `Ex2/Ex2.cpp` | Círculo, octágono, pentágono, pac-man, fatia de pizza, estrela e espiral via equação paramétrica (teclas 1–7) |
| `Ex3/Ex3.cpp` | Triângulo com cores por vértice (P1=vermelho, P2=verde, P3=azul) usando VBO entrelaçado |
| `Ex4/Ex4.cpp` | Cena de uma casa com múltiplos VAOs e primitivas variadas (GL_TRIANGLES, GL_LINE_LOOP, GL_LINES, GL_TRIANGLE_FAN) |

---

## Informações Técnicas

- **Linguagem:** C++ (C++17)
- **API Gráfica:** OpenGL 4.0
- **Dependências:** GLFW, GLAD, GLM
- **IDE/Compilador:** Visual Studio Code / MinGW-UCRT64 / CMake
- **Plataforma-alvo:** Windows

---

## Checklist de Requisitos

- [x] Ex1 a) Dois triângulos apenas preenchidos
- [x] Ex1 b) Dois triângulos apenas com contorno
- [x] Ex1 c) Dois triângulos apenas como pontos
- [x] Ex1 d) Os três modos de desenho juntos
- [x] Ex2 — Círculo via equação paramétrica
- [x] Ex2 a) Octágono
- [x] Ex2 b) Pentágono
- [x] Ex2 c) Pac-Man
- [x] Ex2 d) Fatia de pizza
- [x] Ex2 e) Estrela (DESAFIO 1)
- [x] Ex2 f) Espiral (DESAFIO 2)
- [x] Ex3 — Triângulo com cores por vértice (VBO entrelaçado, 2 atributos)
- [x] Ex4 — Desenho livre com múltiplos VAOs e primitivas diferentes
