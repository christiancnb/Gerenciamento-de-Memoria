/*
 * Questão bônus - Algoritmo de substituição de página LRU
 * Disciplina: Sistemas Operacionais
 *
 * Compilar:  javac LRU.java
 * Executar:  java LRU
 *
 * Recebe a mesma entrada da Questão 3 (frames e sequência). Se o
 * usuário apertar ENTER, usa o exemplo do enunciado.
 *
 * Ideia do LRU (Least Recently Used): quando é preciso substituir,
 * sai a página que está há MAIS TEMPO SEM SER USADA. Para isso, cada
 * página carregada guarda o "instante" (timestamp) do seu último uso
 * em um HashMap<página, timestamp>. A cada referência o timestamp é
 * atualizado; a vítima é a página com o menor timestamp.
 *
 * Ao final, o programa roda também o FIFO sobre a mesma entrada e
 * compara os dois algoritmos.
 */

import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Queue;
import java.util.Scanner;

public class LRU {

    static final int FRAMES_PADRAO = 3;
    static final String SEQUENCIA_PADRAO = "7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2";

    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        System.out.println("=== Questão bônus: Substituição de página LRU ===");

        int frames = lerFrames(sc);
        List<Integer> refs = lerReferencias(sc);

        int faltasLRU = simularLRU(frames, refs, true);
        int faltasFIFO = simularFIFO(frames, refs);

        compararResultados(frames, refs, faltasFIFO, faltasLRU);
    }

    /* ---------------------------- Entrada ---------------------------- */

    static int lerFrames(Scanner sc) {
        while (true) {
            System.out.print("Número de frames [ENTER = " + FRAMES_PADRAO + "]: ");
            if (!sc.hasNextLine()) return FRAMES_PADRAO;
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

    static List<Integer> lerReferencias(Scanner sc) {
        while (true) {
            System.out.print("Sequência de referências [ENTER = " + SEQUENCIA_PADRAO + "]: ");
            String linha = sc.hasNextLine() ? sc.nextLine().trim() : "";
            if (linha.isEmpty()) linha = SEQUENCIA_PADRAO;
            try {
                List<Integer> lista = new ArrayList<>();
                for (String parte : linha.split("[,\\s]+")) {
                    if (parte.isEmpty()) continue;
                    int p = Integer.parseInt(parte);
                    if (p < 0) throw new NumberFormatException();
                    lista.add(p);
                }
                if (!lista.isEmpty()) return lista;
                System.out.println("  Erro: a sequência está vazia.");
            } catch (NumberFormatException e) {
                System.out.println("  Erro: use apenas números inteiros não negativos.");
            }
        }
    }

    /* ------------------------------ LRU ------------------------------ */

    /**
     * Simula o LRU. A lista "frames" guarda as páginas nos quadros físicos
     * (a página substituída é trocada NA MESMA posição, como no hardware).
     * O mapa "ultimoUso" guarda, para cada página carregada, o instante da
     * última referência. Retorna o número de faltas de página.
     */
    static int simularLRU(int numFrames, List<Integer> refs, boolean imprimir) {
        List<Integer> frames = new ArrayList<>();
        Map<Integer, Integer> ultimoUso = new HashMap<>();
        int faltas = 0;

        if (imprimir) {
            System.out.println("\nFrames: " + numFrames + " | Referências: " + refs + "\n");
        }

        for (int tempo = 0; tempo < refs.size(); tempo++) {
            int pagina = refs.get(tempo);
            String resultado;

            if (ultimoUso.containsKey(pagina)) {
                // ACERTO: só atualiza o instante do último uso
                resultado = "Page Hit";
            } else {
                faltas++;
                if (frames.size() < numFrames) {
                    frames.add(pagina);                 // há frame livre
                    resultado = "Page Fault";
                } else {
                    // procura a página com o MENOR timestamp (menos recentemente usada)
                    int vitima = frames.get(0);
                    for (int p : frames) {
                        if (ultimoUso.get(p) < ultimoUso.get(vitima)) vitima = p;
                    }
                    frames.set(frames.indexOf(vitima), pagina);  // troca no mesmo quadro
                    ultimoUso.remove(vitima);
                    resultado = "Page Fault (substituiu " + vitima + ")";
                }
            }
            ultimoUso.put(pagina, tempo);   // registra o uso atual

            if (imprimir) {
                System.out.printf("t=%-3dReferência: %-3d| Frames: %-14s| %-27s| Total Faults: %d%n",
                        tempo, pagina, frames.toString(), resultado, faltas);
            }
        }
        return faltas;
    }

    /* ------------------- FIFO (para a comparação) -------------------- */

    /** Mesmo algoritmo da Questão 3, sem impressão passo a passo. */
    static int simularFIFO(int numFrames, List<Integer> refs) {
        Queue<Integer> fila = new ArrayDeque<>();
        int faltas = 0;
        for (int pagina : refs) {
            if (!fila.contains(pagina)) {
                faltas++;
                if (fila.size() == numFrames) fila.poll();  // sai a mais antiga
                fila.add(pagina);
            }
        }
        return faltas;
    }

    /* --------------------------- Comparação -------------------------- */

    static void compararResultados(int frames, List<Integer> refs, int fFIFO, int fLRU) {
        int total = refs.size();
        System.out.println("\n================ Comparação FIFO x LRU ================");
        System.out.printf("%-10s%15s%20s%n", "Algoritmo", "Faltas", "Taxa de faltas");
        System.out.printf("%-10s%15d%19.2f%%%n", "FIFO", fFIFO, 100.0 * fFIFO / total);
        System.out.printf("%-10s%15d%19.2f%%%n", "LRU", fLRU, 100.0 * fLRU / total);
        System.out.println("-------------------------------------------------------");

        if (fLRU < fFIFO) {
            System.out.printf("O LRU teve %d falta(s) a menos (%.1f%% menos que o FIFO).%n",
                    fFIFO - fLRU, 100.0 * (fFIFO - fLRU) / fFIFO);
        } else if (fLRU > fFIFO) {
            System.out.printf("O FIFO teve %d falta(s) a menos nesta sequência.%n", fLRU - fFIFO);
        } else {
            System.out.println("Os dois algoritmos tiveram o mesmo número de faltas.");
        }

        // Variação do número de frames: mostra a tendência de cada algoritmo
        System.out.println("\nFaltas variando o número de frames (mesma sequência):");
        System.out.printf("%-8s%8s%8s%n", "Frames", "FIFO", "LRU");
        for (int f = 1; f <= Math.max(frames + 2, 5); f++) {
            System.out.printf("%-8d%8d%8d%n", f, simularFIFO(f, refs), simularLRU(f, refs, false));
        }

        System.out.println("\nAnálise:");
        System.out.println("- O FIFO considera só a ORDEM DE CHEGADA: pode expulsar uma página");
        System.out.println("  muito usada só porque ela é antiga (ex.: a página 0 nesta sequência).");
        System.out.println("- O LRU usa o HISTÓRICO RECENTE (localidade temporal): páginas usadas");
        System.out.println("  há pouco tendem a ser usadas de novo, então ficam na memória.");
        System.out.println("- O LRU não sofre a anomalia de Belady (mais frames nunca aumentam as");
        System.out.println("  faltas); o FIFO pode sofrer.");
        System.out.println("- Custo: o LRU precisa atualizar um registro a CADA acesso, o que é caro");
        System.out.println("  em hardware real; por isso SOs usam aproximações (bit de referência,");
        System.out.println("  algoritmo do relógio/segunda chance).");
    }
}
