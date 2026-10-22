# Gerenciamento de Memória em Sistemas Operacionais

## Estrutura

| Questão | Arquivo | Linguagem |
|---|---|---|
| 1 – Alocação estática vs. dinâmica | `q1_alocacao/alocacao.c` | C |
| 2 – Fragmentação (partições fixas, First-Fit) | `q2_fragmentacao/fragmentacao.py` | Python |
| 3 – Substituição de página FIFO | `q3_fifo/FIFO.java` | Java |
| 4 – Garbage Collection | `q4_gc/gc_demo.py` | Python |
| 5 – Desempenho pilha x heap | `q5_desempenho/desempenho.c` | C |
| Bônus – Substituição de página LRU | `bonus_lru/LRU.java` | Java |

- `prints/` – capturas de tela das execuções (`.png`) e a saída em texto (`.txt`)
- `Trabalho_SO_Gerenciamento_de_Memoria.pdf` – documento com as respostas teóricas, códigos, capturas e análises

## Como compilar e executar

Requisitos: GCC (Linux/macOS, ou MinGW-w64 no Windows), JDK 17+ e Python 3.

```bash
# Questão 1
cd q1_alocacao
gcc -Wall -Wextra -o alocacao alocacao.c
./alocacao

# Questão 2
cd q2_fragmentacao
python3 fragmentacao.py

# Questão 3 (ENTER nas perguntas usa o exemplo do enunciado)
cd q3_fifo
javac FIFO.java
java FIFO

# Questão 4
cd q4_gc
python3 gc_demo.py

# Questão 5 (compile SEM otimização: -O0)
cd q5_desempenho
gcc -O0 -Wall -Wextra -o desempenho desempenho.c -lm
./desempenho

# Bônus
cd bonus_lru
javac LRU.java
java LRU
```

### Observações para Windows

- Os executáveis gerados pelo GCC são `alocacao.exe` e `desempenho.exe`.
- Use `python` em vez de `python3`.
- Se os acentos aparecerem errados no terminal, rode `chcp 65001` antes ou use
  `java -Dstdout.encoding=UTF-8 FIFO`.
- Com JDK anterior ao 18, compile com `javac -encoding UTF-8 FIFO.java`.

## Resultados esperados (3 frames, sequência do enunciado)

| Algoritmo | Faltas | Taxa |
|---|---|---|
| FIFO | 10 | 76,92% |
| LRU | 9 | 69,23% |

Questão 2: fragmentação interna total de **80 unidades**; P5 (350) não é alocado.
