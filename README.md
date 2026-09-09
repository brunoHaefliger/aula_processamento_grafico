# Processamento Gráfico: Fundamentos 2026/2

Repositório com os projetos desenvolvidos para a Atividade Acadêmica **Processamento Gráfico: Fundamentos**, parte do curso de graduação em **Ciência da Computação da Unisinos**.

👤 **Nome Completo:** Bruno Haefliger

---

## 📂 Estrutura do Repositório

| Projeto | Engine / Linguagem | Descrição breve |
| --- | --- | --- |
| `Lista 1` | C++/OpenGL | Primitivas gráficas, shaders e buffers (VBO/VAO) |

---

## ⚙️ Como compilar

O projeto usa **CMake** com **MSYS2 UCRT64** no Windows.

### Pré-requisitos
- [MSYS2](https://www.msys2.org/) com toolchain UCRT64
- CMake 3.11+
- Os arquivos `glad.h`, `khrplatform.h` e `glad.c` já estão incluídos no repositório

### Passos
```bash
# Na raiz do repositório
cmake -B build
cmake --build build
```

Os executáveis são gerados na pasta `build/`.

---

> 💡 Este repositório permanecerá **público** até o final da disciplina para fins de avaliação.
