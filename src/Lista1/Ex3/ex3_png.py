"""
Ex3 - Triângulo com cores por vértice (VBO entrelaçado)
Gera ex3_triangulo_cores.png
"""

import moderngl
import numpy as np
from PIL import Image

WIDTH, HEIGHT = 800, 600

VERTEX_SHADER = """
#version 330
in vec3 position;
in vec3 color;
out vec3 vColor;
void main() {
    float aspect = float(800) / float(600);
    gl_Position = vec4(position.x / aspect, position.y, position.z, 1.0);
    vColor = color;
}
"""

FRAGMENT_SHADER = """
#version 330
in vec3 vColor;
out vec4 fragColor;
void main() {
    fragColor = vec4(vColor, 1.0);
}
"""

ctx = moderngl.create_standalone_context()

prog = ctx.program(vertex_shader=VERTEX_SHADER, fragment_shader=FRAGMENT_SHADER)

# Entrelaçado: x y z  r g b  (6 floats por vértice)
vertices = np.array([
     0.0,  0.5, 0.0,  1.0, 0.0, 0.0,   # P1 - topo        - vermelho
    -0.5, -0.5, 0.0,  0.0, 1.0, 0.0,   # P2 - baixo esq   - verde
     0.5, -0.3, 0.0,  0.0, 0.0, 1.0,   # P3 - baixo dir   - azul
], dtype='f4')

vbo = ctx.buffer(vertices.tobytes())
vao = ctx.vertex_array(prog, [(vbo, '3f 3f', 'position', 'color')])

fbo = ctx.simple_framebuffer((WIDTH, HEIGHT))
fbo.use()
fbo.clear(0.1, 0.1, 0.1, 1.0)

vao.render(moderngl.TRIANGLES)

data = fbo.read(components=3)
img  = Image.frombytes('RGB', (WIDTH, HEIGHT), data)
img  = img.transpose(Image.FLIP_TOP_BOTTOM)
img.save('ex3_triangulo_cores.png')
print("Salvo: ex3_triangulo_cores.png")
