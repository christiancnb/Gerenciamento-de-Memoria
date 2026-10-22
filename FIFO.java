/*
 * Questão 3 - Algoritmo de substituição de página FIFO
 * Disciplina: Sistemas Operacionais
 *
 * Compilar:  javac FIFO.java
 * Executar:  java FIFO
 *
 * O programa pede o número de frames e a sequência de referências.
 * Se o usuário apenas apertar ENTER, são usados os valores do exemplo
 * do enunciado (3 frames e 7,0,1,2,0,3,0,4,2,3,0,3,2).
 *
 * Ideia do FIFO: a página que está há MAIS TEMPO na memória é a
 * primeira a sair. Uma fila (Queue) representa exatamente isso:
 * entra no fim, sai pelo começo.
 */

import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Queue;
import java.util.Scanner;
import java.util.Set;

public class FIFO {

    static final int FRAMES_PADRAO = 3;
    static final String SEQUENCIA_PADRAO = "7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2";

    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        System.out.println("=== Questão 3: Substituição de página FIFO ===");

        int frames = lerFrames(sc);
        List<Integer> referencias = lerReferencias(sc);

        simular(frames, referencias);
    }

    /** Lê o número de frames, repetindo a pergunta até receber um inteiro positivo. */
    static int lerFrames(Scanner sc) {
        while (true) {
            System.out.print("Número de frames [ENTER = " + FRAMES_PADRAO + "]: ");
            if (!sc.hasNextLine()) return FRAMES_PADRAO;   // entrada encerrada
            String linha = sc.nextLine().trim();
            if (linha.isEmpty()) return FRAMES_PADRAO;
            try {
                int n = Integer.parseInt(linha);
                if (n > 0) return n;
                System.out.println("  Erro: o número de frames deve ser maior que zero.");
            } catch (NumberFormatException e) {
                System.out.println("  Erro: digite um número inteiro.");
            }
        }
    }

    /** Lê a sequência de páginas separadas por vírgula ou espaço. */
    static List<Integer> lerReferencias(Scanner sc) {
        while (true) {
            System.out.print("Sequência de referências [ENTER = " + SEQUENCIA_PADRAO + "]: ");
            String linha = sc.hasNextLine() ? sc.nextLine().trim() : "";
            if (linha.isEmpty()) linha = SEQUENCIA_PADRAO;
            try {
                List<Integer> lista = new ArrayList<>();
                for (String parte : linha.split("[,\\s]+")) {
                    if (parte.isEmpty()) continue;
                    int pagina = Integer.parseInt(parte);
                    if (pagina < 0) throw new NumberFormatException();
                    lista.add(pagina);
                }
                if (!lista.isEmpty()) return lista;
                System.out.println("  Erro: a sequência está vazia.");
            } catch (NumberFormatException e) {
                System.out.println("  Erro: use apenas números inteiros não negativos.");
            }
        }
    }

    /** Executa a simulação FIFO, imprime cada passo e devolve o total de faltas. */
    static int simular(int numFrames, List<Integer> referencias) {
        Queue<Integer> fila = new ArrayDeque<>();   // ordem de chegada das páginas
        Set<Integer> naMemoria = new HashSet<>();   // consulta rápida: a página está carregada?
        int faltas = 0;

        System.out.println("\nFrames: " + numFrames + " | Referências: " + referencias + "\n");

        for (int pagina : referencias) {
            String resultado;

            if (naMemoria.contains(pagina)) {
                // ACERTO: a página já está em algum frame; no FIFO a ordem não muda
                resultado = "Page Hit";
            } else {
                // FALTA: a página precisa ser carregada
                faltas++;
                if (fila.size() < numFrames) {
                    // ainda há frame livre
                    resultado = "Page Fault";
                } else {
                    // memória cheia: remove a página mais antiga (início da fila)
                    int vitima = fila.poll();
                    naMemoria.remove(vitima);
                    resultado = "Page Fault (substituiu " + vitima + ")";
                }
                fila.add(pagina);          // a nova página entra no fim da fila
                naMemoria.add(pagina);
            }

            System.out.printf("Referência: %-3d| Frames: %-14s| %-27s| Total Faults: %d%n",
                    pagina, fila.toString(), resultado, faltas);
        }

        // Estatísticas finais
        int total = referencias.size();
        int acertos = total - faltas;
        System.out.println("\n----- Estatísticas -----");
        System.out.println("Total de referências : " + total);
        System.out.println("Faltas de página     : " + faltas);
        System.out.println("Acertos (hits)       : " + acertos);
        System.out.printf("Taxa de faltas       : %d/%d = %.2f%%%n", faltas, total, 100.0 * faltas / total);
        System.out.printf("Taxa de acertos      : %.2f%%%n", 100.0 * acertos / total);
        return faltas;
    }
}
