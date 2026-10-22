/*
 * Questão 5 - Comparação de desempenho: alocação na pilha x no heap
 * Disciplina: Sistemas Operacionais
 *
 * Compilar:  gcc -O0 -Wall -Wextra -o desempenho desempenho.c
 * Executar:  ./desempenho          (Windows: desempenho.exe)
 *
 * IMPORTANTE: compile com -O0 (sem otimização). Com -O2 o compilador
 * percebe que as variáveis locais não servem para nada e pode apagar
 * o laço inteiro, fazendo o teste da pilha medir "zero".
 *
 * Testes (cada um repetido REPETICOES vezes; mostramos a média):
 *   1. PILHA : chamar 1.000.000 de vezes uma função que cria um objeto
 *              local (struct de 64 bytes). Ao retornar, ele some sozinho.
 *   2. HEAP A: 1.000.000 de vezes malloc + uso + free do mesmo tamanho.
 *   3. HEAP B: alocar 1.000.000 de objetos e só depois liberar todos
 *              (padrão mais realista: vários objetos vivos ao mesmo tempo).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define N_OBJETOS   1000000   /* 1 milhão de alocações por teste */
#define REPETICOES  10        /* quantas vezes cada teste é repetido */

/* Objeto de 64 bytes usado nos três testes (mesmo tamanho = comparação justa) */
typedef struct {
    int valores[16];
} Objeto;

/* "volatile" obriga o compilador a realmente fazer as escritas/leituras */
static volatile int sumidouro;

/* ---------------------------------------------------------------- */
/* Alocação na PILHA: o objeto é variável local da função.          */
/* Alocar = o processador apenas subtrai 64 do registrador de pilha */
/* (stack pointer) na entrada da função e soma de volta na saída.   */
/* "noinline" impede que a função seja copiada para dentro do laço. */
/* ---------------------------------------------------------------- */
__attribute__((noinline))
static void funcao_com_local(int i)
{
    Objeto local;                    /* criado na pilha */
    local.valores[0] = i;
    local.valores[15] = i * 2;
    sumidouro = local.valores[0] + local.valores[15];
}                                    /* destruído aqui, automaticamente */

static double teste_pilha(void)
{
    clock_t ini = clock();
    for (int i = 0; i < N_OBJETOS; i++) {
        funcao_com_local(i);
    }
    return (double)(clock() - ini) * 1000.0 / CLOCKS_PER_SEC;   /* ms */
}

/* ---------------------------------------------------------------- */
/* Alocação no HEAP (A): malloc e free a cada iteração.             */
/* O alocador precisa procurar um bloco livre, registrar o tamanho, */
/* manter suas listas internas e, no free, devolver o bloco.        */
/* ---------------------------------------------------------------- */
static double teste_heap_individual(void)
{
    clock_t ini = clock();
    for (int i = 0; i < N_OBJETOS; i++) {
        Objeto *obj = malloc(sizeof(Objeto));
        if (obj == NULL) {
            fprintf(stderr, "Erro: malloc falhou na iteração %d\n", i);
            exit(EXIT_FAILURE);
        }
        obj->valores[0] = i;
        obj->valores[15] = i * 2;
        sumidouro = obj->valores[0] + obj->valores[15];
        free(obj);
    }
    return (double)(clock() - ini) * 1000.0 / CLOCKS_PER_SEC;
}

/* ---------------------------------------------------------------- */
/* Alocação no HEAP (B): 1 milhão de objetos vivos ao mesmo tempo.  */
/* Força o alocador a pedir mais memória ao SO (brk/mmap) e a       */
/* gerenciar muitos blocos; é o cenário que mais pesa.              */
/* ---------------------------------------------------------------- */
static double teste_heap_em_lote(void)
{
    Objeto **ptrs = malloc(N_OBJETOS * sizeof(Objeto *));
    if (ptrs == NULL) {
        fprintf(stderr, "Erro: não foi possível alocar o vetor de ponteiros\n");
        exit(EXIT_FAILURE);
    }

    clock_t ini = clock();
    for (int i = 0; i < N_OBJETOS; i++) {
        ptrs[i] = malloc(sizeof(Objeto));
        if (ptrs[i] == NULL) {
            fprintf(stderr, "Erro: malloc falhou na iteração %d\n", i);
            for (int j = 0; j < i; j++) free(ptrs[j]);   /* limpa o que já foi alocado */
            free(ptrs);
            exit(EXIT_FAILURE);
        }
        ptrs[i]->valores[0] = i;
    }
    for (int i = 0; i < N_OBJETOS; i++) {
        free(ptrs[i]);
    }
    double ms = (double)(clock() - ini) * 1000.0 / CLOCKS_PER_SEC;

    free(ptrs);   /* o vetor auxiliar fica fora da medição */
    return ms;
}

/* Executa um teste várias vezes e calcula média, desvio padrão, mínimo e máximo */
typedef struct { double media, desvio, min, max; } Estatistica;

static Estatistica medir(const char *nome, double (*teste)(void))
{
    double tempos[REPETICOES];
    Estatistica e = {0.0, 0.0, 1e18, 0.0};

    printf("%-28s", nome);
    for (int r = 0; r < REPETICOES; r++) {
        tempos[r] = teste();
        e.media += tempos[r];
        if (tempos[r] < e.min) e.min = tempos[r];
        if (tempos[r] > e.max) e.max = tempos[r];
        printf("%7.2f", tempos[r]);
    }
    printf("\n");

    e.media /= REPETICOES;
    for (int r = 0; r < REPETICOES; r++) {
        e.desvio += (tempos[r] - e.media) * (tempos[r] - e.media);
    }
    e.desvio = sqrt(e.desvio / REPETICOES);
    return e;
}

static void comparar(const char *nome, Estatistica heap, Estatistica pilha)
{
    double perc = (heap.media - pilha.media) / pilha.media * 100.0;
    printf("%-26s %+8.1f%% em relação à pilha (%.1fx o tempo)\n",
           nome, perc, heap.media / pilha.media);
}

int main(void)
{
    printf("=== Questão 5: Pilha (stack) x Heap ===\n");
    printf("%d alocações de %zu bytes por teste, %d repetições cada.\n\n",
           N_OBJETOS, sizeof(Objeto), REPETICOES);

    /* Aquecimento: a primeira execução paga custos únicos (páginas sendo
       mapeadas pelo SO, cache frio), por isso não entra na média. */
    teste_pilha();
    teste_heap_individual();

    printf("Tempos de cada repetição (ms):\n");
    Estatistica pilha = medir("Pilha (var. local)", teste_pilha);
    Estatistica heapA = medir("Heap A (malloc+free)", teste_heap_individual);
    Estatistica heapB = medir("Heap B (lote 1 mi objetos)", teste_heap_em_lote);

    printf("\nResumo (ms)                      Média    Desvio       Mín       Máx\n");
    printf("%-28s%10.2f%10.2f%10.2f%10.2f\n", "Pilha", pilha.media, pilha.desvio, pilha.min, pilha.max);
    printf("%-28s%10.2f%10.2f%10.2f%10.2f\n", "Heap A (malloc+free)", heapA.media, heapA.desvio, heapA.min, heapA.max);
    printf("%-28s%10.2f%10.2f%10.2f%10.2f\n", "Heap B (lote)", heapB.media, heapB.desvio, heapB.min, heapB.max);

    printf("\nDiferença percentual de desempenho:\n");
    comparar("Heap A (malloc+free):", heapA, pilha);
    comparar("Heap B (lote):", heapB, pilha);

    /*
     * INTERPRETAÇÃO DOS RESULTADOS
     *
     * - Pilha: alocar e liberar é só mover o stack pointer (uma subtração
     *   e uma soma). Não há busca por espaço livre nem metadados, e a pilha
     *   está quase sempre no cache do processador.
     *
     * - Heap A: cada malloc/free executa o código do alocador (glibc
     *   ptmalloc no Linux): consultar listas de blocos livres ("bins"),
     *   gravar o cabeçalho do bloco, verificar consistência. Como o mesmo
     *   bloco é reutilizado em toda iteração (tcache), este é o MELHOR caso
     *   do heap, e mesmo assim costuma ser algumas vezes mais lento.
     *
     * - Heap B: com 1 milhão de objetos vivos (~80 MB com os cabeçalhos),
     *   o alocador precisa pedir memória ao sistema operacional (brk/mmap),
     *   o SO precisa mapear páginas novas (page faults na primeira escrita)
     *   e os blocos ficam espalhados, piorando o uso de cache. Por isso é
     *   o mais lento.
     *
     * POSSÍVEIS VARIAÇÕES
     * - Os números mudam entre máquinas, SOs (o alocador do Windows é
     *   diferente do glibc), compiladores e níveis de otimização.
     * - Outros processos, escalonamento, frequência variável da CPU e
     *   estado do cache causam diferenças entre repetições; por isso
     *   repetimos e mostramos média e desvio padrão.
     * - Com -O2 o teste da pilha pode ser otimizado até quase zero.
     * - clock() mede tempo de CPU com resolução limitada (1 ms no Windows).
     */
    printf("\nConclusão: a pilha é a mais rápida porque alocar é só mover o\n"
           "stack pointer; o heap paga o custo do alocador e, com muitos\n"
           "objetos vivos, também o custo de pedir memória ao SO.\n");

    return EXIT_SUCCESS;
}
