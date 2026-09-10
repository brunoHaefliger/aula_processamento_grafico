"""
Roda todos os exercícios da Lista 1 e gera os PNGs.
Execute a partir de qualquer pasta:
    python run_all.py
"""

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).parent

scripts = [
    ROOT / "Ex1" / "ex1_png.py",
    ROOT / "Ex2" / "ex2_png.py",
    ROOT / "Ex3" / "ex3_png.py",
    ROOT / "Ex4" / "ex4_png.py",
]

for script in scripts:
    print(f"\n{'='*40}")
    print(f"  {script.parent.name}")
    print(f"{'='*40}")
    result = subprocess.run([sys.executable, str(script)], cwd=script.parent)
    if result.returncode != 0:
        print(f"[ERRO] {script.name} falhou com código {result.returncode}")

print("\nPronto! PNGs salvos em cada pasta de exercício.")
