"""
Ex1 - Dois triângulos com modos de desenho
Gera 4 PNGs (fill, contorno, pontos, todos) sem abrir janela.
"""

import moderngl
import numpy as np
from PIL import Image

WIDTH, HEIGHT = 800, 600

VERTEX_SHADER = """
#version 330
in vec3 position;
void main() {
    gl_Position = vec4(position, 1.0);
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
ctx.point_size = 12.0

prog = ctx.program(vertex_shader=VERTEX_SHADER, fragment_shader=FRAGMENT_SHADER)

vertices = np.array([
    # Triângulo esquerdo — v0, v1, v2
    -0.5,  0.5, 0.0,   # v0
    -0.5, -0.5, 0.0,   # v1
     0.0,  0.0, 0.0,   # v2
    # Triângulo direito — v3, v4, v5
     0.0,  0.0, 0.0,   # v3
     0.5, -0.5, 0.0,   # v4
     0.5,  0.5, 0.0,   # v5
], dtype='f4')

vbo = ctx.buffer(vertices.tobytes())
vao = ctx.vertex_array(prog, [(vbo, '3f', 'position')])

fbo = ctx.simple_framebuffer((WIDTH, HEIGHT))
fbo.use()

BG = (0.15, 0.15, 0.15, 1.0)

def save(filename):
    data = fbo.read(components=3)
    img = Image.frombytes('RGB', (WIDTH, HEIGHT), data)
    img = img.transpose(Image.FLIP_TOP_BOTTOM)
    img.save(filename)
    print(f"Salvo: {filename}")

# a) Apenas preenchido
fbo.clear(*BG)
ctx.wireframe = False
prog['inputColor'].value = (0.2, 0.5, 0.9, 1.0)
vao.render(moderngl.TRIANGLES)
save('ex1_a_fill.png')

# b) Apenas contorno
fbo.clear(*BG)
ctx.wireframe = True
prog['inputColor'].value = (1.0, 1.0, 1.0, 1.0)
vao.render(moderngl.TRIANGLES)
ctx.wireframe = False
save('ex1_b_contorno.png')

# c) Apenas pontos
fbo.clear(*BG)
prog['inputColor'].value = (1.0, 0.5, 0.0, 1.0)
vao.render(moderngl.POINTS)
save('ex1_c_pontos.png')

# d) Os três juntos
fbo.clear(*BG)
ctx.wireframe = False
prog['inputColor'].value = (0.2, 0.5, 0.9, 1.0)
vao.render(moderngl.TRIANGLES)
ctx.wireframe = True
prog['inputColor'].value = (1.0, 1.0, 1.0, 1.0)
vao.render(moderngl.TRIANGLES)
ctx.wireframe = False
prog['inputColor'].value = (1.0, 0.3, 0.0, 1.0)
vao.render(moderngl.POINTS)
save('ex1_d_todos.png')

print("Pronto!")
