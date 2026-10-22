"""
Questão 4 - Demonstração do Garbage Collector do Python (CPython)
Disciplina: Sistemas Operacionais

Executar:  python3 gc_demo.py      (Windows: python gc_demo.py)

O CPython libera memória de dois jeitos:
  1) CONTAGEM DE REFERÊNCIAS: cada objeto sabe quantas referências
     apontam para ele; quando a contagem chega a zero, ele é destruído
     NA HORA (por isso o __del__ aparece imediatamente).
  2) COLETOR GERACIONAL (módulo gc): encontra CICLOS de objetos que se
     referenciam entre si. Nesses ciclos a contagem nunca chega a zero,
     então só o coletor consegue liberá-los.

Cenários demonstrados:
  1. Destruição automática ao remover a última referência (del)
  2. Destruição ao sair do escopo de uma função
  3. Referência circular entre dois objetos (só o gc resolve)
  4. Objeto que referencia a si mesmo
  5. Coleta geracional automática disparada pelo limite (threshold)
  + estatísticas com gc.get_stats()
"""

import gc
import sys
import tracemalloc

TAMANHO_DADOS = 1_000_000   # elementos da lista "dados" (~8 MB por objeto)


class Objeto:
    """Objeto que ocupa bastante memória e avisa quando nasce e morre."""

    def __init__(self, nome, tamanho=TAMANHO_DADOS):
        self.nome = nome
        self.dados = [0] * tamanho   # simula um uso grande de memória
        self.ref = None              # usada para criar ciclos
        print(f"   [+] Objeto '{self.nome}' criado ({tamanho:,} elementos)".replace(",", "."))

    def __del__(self):
        # Chamado quando o objeto é efetivamente destruído
        print(f"   [-] Objeto '{self.nome}' destruído")


def memoria_mb():
    """Memória atualmente alocada pelo Python (medida pelo tracemalloc)."""
    atual, _pico = tracemalloc.get_traced_memory()
    return atual / (1024 * 1024)


def titulo(texto):
    print("\n" + "=" * 64)
    print(texto)
    print("=" * 64)


# ----------------------------------------------------------------------
def cenario1_contagem_referencias():
    titulo("CENÁRIO 1 - Contagem de referências e 'del'")
    a = Objeto("A")
    # getrefcount mostra 1 a mais, porque o próprio argumento da função
    # cria uma referência temporária.
    print(f"   refcount(A) com 1 variável: {sys.getrefcount(a)}")

    b = a           # segunda referência ao MESMO objeto
    lista = [a]     # terceira referência (dentro de uma lista)
    print(f"   refcount(A) após b = a e lista = [a]: {sys.getrefcount(a)}")
    print(f"   memória em uso: {memoria_mb():.1f} MB")

    del b
    print(f"   del b      -> refcount(A) = {sys.getrefcount(a)}")
    lista.clear()
    print(f"   lista.clear() -> refcount(A) = {sys.getrefcount(a)}")
    print("   del a (última referência) ...")
    del a           # contagem chega a 0: destruição IMEDIATA, sem o gc
    print(f"   memória em uso: {memoria_mb():.1f} MB  (liberada na hora)")


# ----------------------------------------------------------------------
def funcao_temporaria():
    local = Objeto("Local")   # só existe dentro desta função
    print(f"   dentro da função, refcount(Local) = {sys.getrefcount(local)}")
    print("   saindo da função ...")
    # Ao retornar, a variável local desaparece e o objeto é destruído


def cenario2_escopo():
    titulo("CENÁRIO 2 - Fim do escopo de uma função")
    funcao_temporaria()
    print("   (o objeto foi destruído assim que a função terminou)")


# ----------------------------------------------------------------------
def cenario3_ciclo():
    titulo("CENÁRIO 3 - Referência circular entre dois objetos")
    gc.disable()   # desliga o coletor automático para observar o problema
    x = Objeto("X")
    y = Objeto("Y")
    x.ref = y      # X aponta para Y
    y.ref = x      # Y aponta para X -> ciclo
    print(f"   refcount(X) = {sys.getrefcount(x)}, refcount(Y) = {sys.getrefcount(y)}")
    print(f"   memória em uso: {memoria_mb():.1f} MB")

    print("   del x, y ...")
    del x, y
    # Nada é impresso: cada objeto ainda tem 1 referência (a do outro).
    print("   -> nenhum __del__ foi chamado: o ciclo continua na memória!")
    print(f"   memória em uso: {memoria_mb():.1f} MB  (lixo inalcançável)")

    print("   chamando gc.collect() ...")
    coletados = gc.collect()   # o coletor detecta o ciclo e o libera
    print(f"   gc.collect() encontrou {coletados} objetos inalcançáveis")
    print("   (além de X e Y, contam os objetos internos deles: listas, dicionários)")
    print(f"   memória em uso: {memoria_mb():.1f} MB")
    gc.enable()


# ----------------------------------------------------------------------
def cenario4_auto_referencia():
    titulo("CENÁRIO 4 - Objeto que referencia a si mesmo")
    gc.disable()
    s = Objeto("Self")
    s.ref = s      # o objeto aponta para ele mesmo
    print(f"   refcount(Self) = {sys.getrefcount(s)}")
    del s
    print("   del s -> nada acontece (a auto-referência mantém a contagem em 1)")
    print("   chamando gc.collect() ...")
    coletados = gc.collect()
    print(f"   gc.collect() coletou {coletados} objetos")
    gc.enable()


# ----------------------------------------------------------------------
class No:
    """Objeto pequeno, sem __del__, para gerar muitos ciclos rapidamente."""
    def __init__(self):
        self.ref = self


def cenario5_geracional():
    titulo("CENÁRIO 5 - Coleta geracional automática")
    print(f"   thresholds (limites) das gerações: {gc.get_threshold()}")
    gc.collect()   # começa com as gerações vazias
    antes = [s["collections"] for s in gc.get_stats()]
    print(f"   contadores atuais (alocações pendentes por geração): {gc.get_count()}")

    # Criamos muitos ciclos SEM chamar gc.collect(): quando as alocações
    # passam do limite da geração 0, o próprio Python dispara a coleta.
    for _ in range(50_000):
        No()   # cada objeto é lixo cíclico assim que é criado

    depois = [s["collections"] for s in gc.get_stats()]
    print("   foram criados 50.000 objetos cíclicos, sem chamar gc.collect()")
    for g in range(3):
        print(f"   geração {g}: {depois[g] - antes[g]} coleta(s) automática(s)")
    print(f"   contadores agora: {gc.get_count()}")
    print("   -> a geração 0 é coletada com frequência; objetos que sobrevivem")
    print("      sobem para as gerações 1 e 2, que são verificadas mais raramente.")


# ----------------------------------------------------------------------
def exibir_estatisticas():
    titulo("ESTATÍSTICAS DO GC - gc.get_stats()")
    for geracao, est in enumerate(gc.get_stats()):
        print(f"   geração {geracao}: coletas = {est['collections']:>5} | "
              f"coletados = {est['collected']:>7} | "
              f"incoletáveis = {est['uncollectable']}")


def main():
    print("=== Questão 4: Garbage Collection em Python ===")
    print(f"Python {sys.version.split()[0]} | gc habilitado: {gc.isenabled()}")
    tracemalloc.start()

    cenario1_contagem_referencias()
    cenario2_escopo()
    cenario3_ciclo()
    cenario4_auto_referencia()
    cenario5_geracional()
    exibir_estatisticas()

    titulo("RESUMO")
    print("   Contagem de referências: imediata, determinística, mas não")
    print("   resolve ciclos (cenários 1 e 2).")
    print("   Coletor geracional: roda periodicamente e recupera ciclos")
    print("   (cenários 3, 4 e 5), ao custo de pausas para varrer objetos.")
    tracemalloc.stop()


if __name__ == "__main__":
    main()
