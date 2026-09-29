"""
Lista 2 - Ex1: Projeção Ortogonal com Coordenadas do Mundo
Janela do mundo: xmin=-10, xmax=10, ymin=-10, ymax=10
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
    # Flat column-major array (OpenGL format)
    return np.array([
        sx,  0,   0,  0,
        0,   sy,  0,  0,
        0,   0,  sz,  0,
        tx,  ty,  tz,  1
    ], dtype='f4')

ctx = moderngl.create_standalone_context()
prog = ctx.program(vertex_shader=VERTEX_SHADER, fragment_shader=FRAGMENT_SHADER)

# Vértices em coordenadas do mundo (-10 a 10)
vertices = np.array([
    -6.0, -5.0, 0.0,
     6.0, -5.0, 0.0,
     0.0,  7.0, 0.0,
], dtype='f4')

vbo = ctx.buffer(vertices.tobytes())
vao = ctx.vertex_array(prog, [(vbo, '3f', 'position')])

fbo = ctx.simple_framebuffer((WIDTH, HEIGHT))
fbo.use()

# Projeção ortogonal: xmin=-10, xmax=10, ymin=-10, ymax=10
proj = make_ortho(-10, 10, -10, 10, -1, 1)
prog['projection'].write(proj.tobytes())

fbo.clear(0.15, 0.15, 0.15, 1.0)
prog['inputColor'].value = (0.2, 0.5, 1.0, 1.0)
vao.render(moderngl.TRIANGLES)

data = fbo.read(components=3)
img = Image.frombytes('RGB', (WIDTH, HEIGHT), data).transpose(Image.FLIP_TOP_BOTTOM)
img.save('ex1_ortho_mundo.png')
print("Salvo: ex1_ortho_mundo.png")
