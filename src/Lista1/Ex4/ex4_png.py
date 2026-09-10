"""
Ex4 - Casa em papel quadriculado com múltiplos VAOs e primitivas diferentes
Gera ex4_cena_casa.png
"""

import moderngl
import numpy as np
from PIL import Image
import math

WIDTH, HEIGHT = 800, 600

VERTEX_SHADER = """
#version 330
in vec3 position;
void main() {
    float aspect = float(800) / float(600);
    gl_Position = vec4(position.x / aspect, position.y, position.z, 1.0);
}
"""

FRAGMENT_SHADER = """
#version 330
uniform vec4 inputColor;
out vec4 color;
void main() {
    color = inputColor;
}
"""

ctx = moderngl.create_standalone_context()
ctx.line_width = 2.0
ctx.point_size = 10.0

prog = ctx.program(vertex_shader=VERTEX_SHADER, fragment_shader=FRAGMENT_SHADER)

fbo = ctx.simple_framebuffer((WIDTH, HEIGHT))
fbo.use()
fbo.clear(0.20, 0.20, 0.20, 1.0)  # fundo cinza escuro (papel quadriculado)

def make_vao(verts):
    vbo = ctx.buffer(np.array(verts, dtype='f4').tobytes())
    return ctx.vertex_array(prog, [(vbo, '3f', 'position')])

def rect(x0, y0, x1, y1):
    return [
        x0, y0, 0.0,  x1, y0, 0.0,  x1, y1, 0.0,
        x0, y0, 0.0,  x1, y1, 0.0,  x0, y1, 0.0,
    ]

# Janela: coordenadas compartilhadas
wx0, wx1, wx2 = -0.38, -0.25, -0.12
wy0, wy1, wy2 = -0.02,  0.07,  0.16
PANE = (0.78, 0.85, 0.20, 1.0)   # amarelo
BLACK = (0.0, 0.0, 0.0, 1.0)

scene = [
    # Corpo da casa (cinza claro)
    (make_vao(rect(-0.45, -0.55, 0.45, 0.22)), moderngl.TRIANGLES,  (0.82, 0.82, 0.82, 1.0)),

    # Telhado (vermelho escuro) — pontas nas laterais da casa, pico um pouco mais alto
    (make_vao([-0.45, 0.22, 0.0,
                0.45, 0.22, 0.0,
                0.00, 0.76, 0.0]),             moderngl.TRIANGLES,  (0.75, 0.12, 0.12, 1.0)),

    # Janela — 4 panos amarelos (mais estreita)
    (make_vao(rect(wx0, wy1, wx1, wy2)),       moderngl.TRIANGLES,  PANE),  # TL
    (make_vao(rect(wx1, wy1, wx2, wy2)),       moderngl.TRIANGLES,  PANE),  # TR
    (make_vao(rect(wx0, wy0, wx1, wy1)),       moderngl.TRIANGLES,  PANE),  # BL
    (make_vao(rect(wx1, wy0, wx2, wy1)),       moderngl.TRIANGLES,  PANE),  # BR

    # Contorno da janela (LINE_LOOP)
    (make_vao([wx0, wy0, 0.0,
               wx2, wy0, 0.0,
               wx2, wy2, 0.0,
               wx0, wy2, 0.0]), moderngl.LINE_LOOP, BLACK),

    # Cruz da janela (LINES)
    (make_vao([wx1, wy0, 0.0,  wx1, wy2, 0.0,   # vertical
               wx0, wy1, 0.0,  wx2, wy1, 0.0]), moderngl.LINES, BLACK),  # horizontal

    # Porta (marrom) — centralizada e um pouco mais baixa
    (make_vao(rect(-0.12, -0.55, 0.12, -0.08)), moderngl.TRIANGLES,  (0.45, 0.25, 0.10, 1.0)),

    # Contorno da porta (LINE_LOOP)
    (make_vao([-0.12, -0.55, 0.0,
                0.12, -0.55, 0.0,
                0.12, -0.08, 0.0,
               -0.12, -0.08, 0.0]), moderngl.LINE_LOOP, BLACK),

    # Contorno do corpo da casa (LINE_LOOP)
    (make_vao([-0.45, -0.55, 0.0,
                0.45, -0.55, 0.0,
                0.45,  0.22, 0.0,
               -0.45,  0.22, 0.0]), moderngl.LINE_LOOP, BLACK),

    # Pontos nos cantos + pico do telhado (POINTS)
    (make_vao([-0.45, -0.55, 0.0,
                0.45, -0.55, 0.0,
                0.45,  0.22, 0.0,
               -0.45,  0.22, 0.0,
                0.00,  0.76, 0.0]), moderngl.POINTS, BLACK),
]

for vao, primitive, color in scene:
    prog['inputColor'].value = color
    vao.render(primitive)

data = fbo.read(components=3)
img  = Image.frombytes('RGB', (WIDTH, HEIGHT), data)
img  = img.transpose(Image.FLIP_TOP_BOTTOM)
img.save('ex4_cena_casa.png')
print("Salvo: ex4_cena_casa.png")
