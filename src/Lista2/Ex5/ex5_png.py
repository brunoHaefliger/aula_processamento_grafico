"""
Lista 2 - Ex4b: Mesma Cena nos 4 Quadrantes
Define ctx.viewport para cada quadrante e repete o triângulo magenta.
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

fbo = ctx.framebuffer(color_attachments=[ctx.texture((WIDTH, HEIGHT), 3)])
fbo.use()

# ── Fundo + grade (viewport completo) ──────────────────────────────────
ctx.viewport = (0, 0, WIDTH, HEIGHT)
prog['projection'].write(make_ortho(0, 800, 600, 0, -1, 1).tobytes())
ctx.clear(0.27, 0.51, 0.71, 1.0)

grade_verts = np.array([
      0, 300, 0,   800, 300, 0,
    400,   0, 0,   400, 600, 0,
], dtype='f4')
vbo_g = ctx.buffer(grade_verts.tobytes())
vao_g = ctx.vertex_array(prog, [(vbo_g, '3f', 'position')])
ctx.line_width = 2.0
prog['inputColor'].value = (0.8, 0.8, 0.8, 1.0)
vao_g.render(moderngl.LINES)

# ── Triângulo em cada quadrante ────────────────────────────────────────
tri_verts = np.array([
    -7.0, -6.0, 0.0,
     7.0, -6.0, 0.0,
     0.0,  7.5, 0.0,
], dtype='f4')
vbo_t = ctx.buffer(tri_verts.tobytes())
vao_t = ctx.vertex_array(prog, [(vbo_t, '3f', 'position')])

prog['projection'].write(make_ortho(-10, 10, -10, 10, -1, 1).tobytes())
prog['inputColor'].value = (1.0, 0.0, 1.0, 1.0)  # magenta

# (x, y) do canto inf-esq de cada quadrante em coords OpenGL (y=0 na base)
quadrantes = [
    (  0, 300),  # superior esquerdo
    (400, 300),  # superior direito
    (  0,   0),  # inferior esquerdo
    (400,   0),  # inferior direito
]
for x, y in quadrantes:
    ctx.viewport = (x, y, 400, 300)
    vao_t.render(moderngl.TRIANGLES)

ctx.viewport = (0, 0, WIDTH, HEIGHT)
data = fbo.read(components=3)
img = Image.frombytes('RGB', (WIDTH, HEIGHT), data).transpose(Image.FLIP_TOP_BOTTOM)
img.save('ex4b_4quadrantes.png')
print("Salvo: ex4b_4quadrantes.png")
