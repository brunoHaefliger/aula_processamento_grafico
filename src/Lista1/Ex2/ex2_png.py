"""
Ex2 - Formas paramétricas
Gera 7 PNGs (círculo, octágono, pentágono, pac-man, pizza, estrela, espiral).
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

prog = ctx.program(vertex_shader=VERTEX_SHADER, fragment_shader=FRAGMENT_SHADER)

fbo = ctx.simple_framebuffer((WIDTH, HEIGHT))
fbo.use()

BG    = (0.15, 0.15, 0.15, 1.0)
COLOR = (0.9, 0.75, 0.1, 1.0)  # amarelo-dourado

def save(filename):
    data = fbo.read(components=3)
    img  = Image.frombytes('RGB', (WIDTH, HEIGHT), data)
    img  = img.transpose(Image.FLIP_TOP_BOTTOM)
    img.save(filename)
    print(f"Salvo: {filename}")

def render(vertices, primitive):
    fbo.clear(*BG)
    vbo = ctx.buffer(np.array(vertices, dtype='f4').tobytes())
    vao = ctx.vertex_array(prog, [(vbo, '3f', 'position')])
    prog['inputColor'].value = COLOR
    vao.render(primitive)
    vao.release()
    vbo.release()

def ngon(cx, cy, r, n):
    v = [(cx, cy, 0.0)]
    for i in range(n + 1):
        a = 2 * math.pi * i / n
        v.append((cx + r * math.cos(a), cy + r * math.sin(a), 0.0))
    return [c for p in v for c in p]

def pacman(cx, cy, r, n, mouth):
    v = [(cx, cy, 0.0)]
    start, end = mouth, 2 * math.pi - mouth
    segs = int((end - start) / (2 * math.pi) * n)
    for i in range(segs + 1):
        a = start + (end - start) * i / segs
        v.append((cx + r * math.cos(a), cy + r * math.sin(a), 0.0))
    return [c for p in v for c in p]

def pizza(cx, cy, r, start, sector, n):
    v = [(cx, cy, 0.0)]
    for i in range(n + 1):
        a = start + sector * i / n
        v.append((cx + r * math.cos(a), cy + r * math.sin(a), 0.0))
    return [c for p in v for c in p]

def star(cx, cy, r_out, r_in, points):
    v = [(cx, cy, 0.0)]
    for i in range(2 * points + 1):
        r = r_out if i % 2 == 0 else r_in
        a = -math.pi / 2 + i * math.pi / points
        v.append((cx + r * math.cos(a), cy + r * math.sin(a), 0.0))
    return [c for p in v for c in p]

def spiral(cx, cy, r_min, r_max, loops, segs):
    v = []
    for i in range(segs + 1):
        t = i / segs
        a = t * 2 * math.pi * loops
        r = r_min + (r_max - r_min) * t
        v.extend([cx + r * math.cos(a), cy + r * math.sin(a), 0.0])
    return v

render(ngon(0, 0, 0.7, 64),                              moderngl.TRIANGLE_FAN)
save('ex2_1_circulo.png')

render(ngon(0, 0, 0.7, 8),                               moderngl.TRIANGLE_FAN)
save('ex2_2_octagono.png')

render(ngon(0, 0, 0.7, 5),                               moderngl.TRIANGLE_FAN)
save('ex2_3_pentagono.png')

render(pacman(0, 0, 0.7, 64, math.pi / 6),               moderngl.TRIANGLE_FAN)
save('ex2_4_pacman.png')

render(pizza(0, 0, 0.7, -math.pi / 4, math.pi / 2, 32), moderngl.TRIANGLE_FAN)
save('ex2_5_pizza.png')

render(star(0, 0, 0.7, 0.28, 5),                         moderngl.TRIANGLE_FAN)
save('ex2_6_estrela.png')

render(spiral(0, 0, 0.05, 0.7, 3, 300),                  moderngl.LINE_STRIP)
save('ex2_7_espiral.png')

print("Pronto!")
