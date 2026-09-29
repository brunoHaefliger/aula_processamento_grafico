"""
Lista 2 - Ex3: Cena com Câmera 2D em Pixels
ortho(0,800,600,0,-1,1): y=0 no TOPO — coords intuitivos para posicionamento.
Cena: céu + chão + casa (corpo, telhado, porta).
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

proj = make_ortho(0, 800, 600, 0, -1, 1)
prog['projection'].write(proj.tobytes())

fbo = ctx.simple_framebuffer((WIDTH, HEIGHT))
fbo.use()

def make_vao(verts_list):
    arr = np.array(verts_list, dtype='f4')
    vbo = ctx.buffer(arr.tobytes())
    return ctx.vertex_array(prog, [(vbo, '3f', 'position')]), len(arr) // 3

# Chão
chao_v = [
      0, 490, 0,  800, 490, 0,  800, 600, 0,
      0, 490, 0,  800, 600, 0,    0, 600, 0,
]
# Corpo da casa
corpo_v = [
    260, 310, 0,  540, 310, 0,  540, 490, 0,
    260, 310, 0,  540, 490, 0,  260, 490, 0,
]
# Telhado
telhado_v = [210, 310, 0,  590, 310, 0,  400, 130, 0]

# Porta
porta_v = [
    360, 395, 0,  440, 395, 0,  440, 490, 0,
    360, 395, 0,  440, 490, 0,  360, 490, 0,
]

vaos = [
    (make_vao(chao_v),    (0.25, 0.65, 0.25, 1.0)),
    (make_vao(corpo_v),   (0.9,  0.85, 0.72, 1.0)),
    (make_vao(telhado_v), (0.72, 0.18, 0.12, 1.0)),
    (make_vao(porta_v),   (0.5,  0.3,  0.1,  1.0)),
]

fbo.clear(0.53, 0.81, 0.98, 1.0)  # azul céu
for (vao, n), color in vaos:
    prog['inputColor'].value = color
    vao.render(moderngl.TRIANGLES)

data = fbo.read(components=3)
img = Image.frombytes('RGB', (WIDTH, HEIGHT), data).transpose(Image.FLIP_TOP_BOTTOM)
img.save('ex3_cena_pixels.png')
print("Salvo: ex3_cena_pixels.png")
