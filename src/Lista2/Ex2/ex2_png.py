"""
Lista 2 - Ex2: Projeção Ortogonal com Coordenadas de Pixel
xmin=0, xmax=800, ymin=600, ymax=0 — y=0 no TOPO (convenção de tela).
"""

import moderngl
import numpy as np
from PIL import Image

WIDTH, HEIGHT = 800, 600

VERTEX_SHADER = """
#version 330
in vec3 position;
uniform mat4 projection;
void main() {
    gl_Position = projection * vec4(position, 1.0);
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

def make_ortho(left, right, bottom, top, near, far):
    sx = 2.0 / (right - left)
    sy = 2.0 / (top - bottom)
    sz = -2.0 / (far - near)
    tx = -(right + left) / (right - left)
    ty = -(top + bottom) / (top - bottom)
    tz = -(far + near) / (far - near)
    return np.array([sx, 0, 0, 0, 0, sy, 0, 0, 0, 0, sz, 0, tx, ty, tz, 1], dtype='f4')

ctx = moderngl.create_standalone_context()
prog = ctx.program(vertex_shader=VERTEX_SHADER, fragment_shader=FRAGMENT_SHADER)

# Triângulo em coordenadas de pixel (y=0 no topo), apontando para cima
vertices = np.array([
    150.0, 440.0, 0.0,
    650.0, 440.0, 0.0,
    400.0,  80.0, 0.0,
], dtype='f4')

vbo = ctx.buffer(vertices.tobytes())
vao = ctx.vertex_array(prog, [(vbo, '3f', 'position')])

fbo = ctx.simple_framebuffer((WIDTH, HEIGHT))
fbo.use()

proj = make_ortho(0, 800, 600, 0, -1, 1)
prog['projection'].write(proj.tobytes())

fbo.clear(0.15, 0.15, 0.15, 1.0)
prog['inputColor'].value = (1.0, 0.85, 0.0, 1.0)  # amarelo
vao.render(moderngl.TRIANGLES)

data = fbo.read(components=3)
img = Image.frombytes('RGB', (WIDTH, HEIGHT), data).transpose(Image.FLIP_TOP_BOTTOM)
img.save('ex2_ortho_pixel.png')
print("Salvo: ex2_ortho_pixel.png")
