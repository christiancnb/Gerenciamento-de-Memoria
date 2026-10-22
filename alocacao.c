/*
 * Questão 1 - Alocação estática vs. dinâmica de memória
 * Disciplina: Sistemas Operacionais
 *
 * Compilar:  gcc -Wall -Wextra -o alocacao alocacao.c
 * Executar:  ./alocacao        (Windows: alocacao.exe)
 *
 * O programa mostra onde ficam, na memória do processo, um array
 * estático (área de dados/BSS, definida em tempo de compilação) e um
 * array dinâmico (heap, obtido em tempo de execução com malloc).
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>   /* intptr_t: inteiro capaz de guardar um endereço */

#define TAM_ESTATICO 5
#define TAM_DINAMICO 10

/*
 * Array ESTÁTICO: declarado fora de qualquer função e com "static".
 * Seu tamanho é fixo e conhecido pelo compilador; ele existe durante
 * toda a execução e fica no segmento de dados (BSS) do executável.
 */
static int vetor_estatico[TAM_ESTATICO];

/* Imprime os valores e o endereço de cada posição de um vetor */
static void imprimir_vetor(const char *nome, const int *v, int tam)
{
    printf("\n%s (%d elementos):\n", nome, tam);
    for (int i = 0; i < tam; i++) {
        printf("  [%d] valor = %2d   endereço = %p\n", i, v[i], (void *)&v[i]);
    }
}

int main(void)
{
    printf("=== Questão 1: Alocação estática vs. dinâmica ===\n");

    /* 1. Preenche o array estático com os valores 1 a 5 */
    for (int i = 0; i < TAM_ESTATICO; i++) {
        vetor_estatico[i] = i + 1;
    }

    /*
     * 2. Aloca DINAMICAMENTE um array de 10 inteiros no heap.
     *    malloc recebe o número de BYTES, por isso multiplicamos
     *    pela quantidade de bytes de um int (sizeof(int)).
     */
    int *vetor_dinamico = (int *)malloc(TAM_DINAMICO * sizeof(int));

    /* Verificação obrigatória: malloc devolve NULL se não houver memória */
    if (vetor_dinamico == NULL) {
        fprintf(stderr, "Erro: falha ao alocar memória com malloc.\n");
        return EXIT_FAILURE;
    }

    /* 3. Preenche o array dinâmico com os valores 10 a 19 */
    for (int i = 0; i < TAM_DINAMICO; i++) {
        vetor_dinamico[i] = 10 + i;
    }

    /* Exibe valores e endereços de cada elemento */
    imprimir_vetor("Array estático (segmento de dados/BSS)", vetor_estatico, TAM_ESTATICO);
    imprimir_vetor("Array dinâmico (heap)", vetor_dinamico, TAM_DINAMICO);

    /* 4. Endereços iniciais (base) de cada array */
    printf("\nEndereço base do array estático: %p\n", (void *)vetor_estatico);
    printf("Endereço base do array dinâmico: %p\n", (void *)vetor_dinamico);

    /*
     * 5. Diferença entre os endereços. Convertemos os ponteiros para
     *    intptr_t para fazer a subtração como números inteiros, já que
     *    subtrair ponteiros de objetos diferentes não é definido em C.
     */
    intptr_t end_est = (intptr_t)vetor_estatico;
    intptr_t end_din = (intptr_t)vetor_dinamico;
    intptr_t diferenca = end_din - end_est;
    if (diferenca < 0) {
        diferenca = -diferenca;
    }

    printf("\nDiferença entre os endereços: %ld bytes (~%.2f KB, ~%.2f MB)\n",
           (long)diferenca, diferenca / 1024.0, diferenca / (1024.0 * 1024.0));
    printf("Tamanho ocupado: estático = %zu bytes | dinâmico = %zu bytes\n",
           sizeof(vetor_estatico), TAM_DINAMICO * sizeof(int));
    printf("\nConclusão: a distância entre os endereços é muito maior que o\n"
           "tamanho dos arrays, mostrando que eles estão em regiões diferentes\n"
           "da memória do processo (dados estáticos x heap).\n");

    /* 6. Libera a memória dinâmica; sem isso haveria vazamento (memory leak) */
    free(vetor_dinamico);
    vetor_dinamico = NULL;   /* evita usar um ponteiro "pendurado" (dangling) */
    printf("\nMemória dinâmica liberada com free().\n");

    return EXIT_SUCCESS;
}
