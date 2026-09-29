# Lista de Exercícios 2 – Câmera 2D e Viewport

## Equipe
- Bruno Haefliger

---

## Descrição do Projeto

Exercícios da disciplina *Processamento Gráfico: Fundamentos* sobre projeção ortogonal 2D e controle de viewport com OpenGL moderna e GLM.

---

## Estrutura do Projeto

| Arquivo | Descrição |
|---|---|
| `Ex1/Ex1.cpp` | Projeção ortogonal com mundo -10 a 10 em ambos os eixos |
| `Ex2/Ex2.cpp` | Projeção ortogonal em coordenadas de pixel (0–800, 0–600, y=0 no topo) |
| `Ex3/Ex3.cpp` | Cena de uma casa usando a câmera 2D em pixels (demonstra utilidade do sistema) |
| `Ex4/Ex4.cpp` | Viewport restrito ao quadrante superior direito da janela |

Cada exercício também possui um script `*_png.py` equivalente (moderngl + Pillow) para gerar o PNG sem compilar C++.

---

## Conceitos Abordados

### Exercício 1 — Coordenadas do Mundo
`ortho(-10, 10, -10, 10, -1, 1)` mapeia o espaço do mundo [-10, 10] ao viewport inteiro.
Um triângulo em (-6,-5), (6,-5), (0,7) é posicionado em unidades de mundo.

### Exercício 2 — Coordenadas de Pixel
`ortho(0, 800, 600, 0, -1, 1)` mapeia pixels diretamente:
- x=0 → borda esquerda, x=800 → borda direita
- y=0 → **topo** da janela, y=600 → base (convenção de tela)

### Exercício 3 — Utilidade da Câmera 2D
Posicionar objetos em pixels é intuitivo: `(400, 300)` é exatamente o centro de uma janela 800×600.
Elimina a necessidade de converter coordenadas de NDC manualmente — fundamental para UI, HUD e jogos 2D.

### Exercício 4 — Viewport Restrito
`glViewport(400, 300, 400, 300)` limita o rasterizador ao quadrante superior direito.
O espaço NDC [-1,1]² é mapeado apenas para essa área; o restante da tela não é tocado.

---

## Checklist de Requisitos

- [x] Ex1 — Câmera ortogonal com mundo -10 a 10
- [x] Ex2 — Câmera ortogonal em coordenadas de pixel (0–800, 0–600)
- [x] Ex3 — Cena desenhada com a câmera 2D em pixels
- [x] Ex4 — Viewport restrito ao quadrante superior direito

---

## Informações Técnicas

- **Linguagem:** C++ (C++17) + Python 3 (scripts PNG)
- **API Gráfica:** OpenGL 4.0 / moderngl (Python)
- **Dependências C++:** GLFW, GLAD, GLM, stb_image_write
- **Dependências Python:** moderngl, numpy, Pillow
