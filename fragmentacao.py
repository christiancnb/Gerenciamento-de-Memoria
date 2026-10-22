"""
Questão 2 - Simulação de fragmentação interna com partições fixas
Disciplina: Sistemas Operacionais

Executar:  python3 fragmentacao.py      (Windows: python fragmentacao.py)

Simula uma memória de 1000 unidades dividida em 5 partições FIXAS
(100, 150, 200, 250 e 300). Cada partição recebe no máximo um processo.
A alocação usa o algoritmo First-Fit: a primeira partição livre que
comporta o processo é escolhida. Como a partição inteira fica reservada,
o espaço que sobra dentro dela é FRAGMENTAÇÃO INTERNA.
"""

TAMANHO_MEMORIA = 1000
TAMANHOS_PARTICOES = [100, 150, 200, 250, 300]

# Cada partição é um dicionário dentro de uma lista (mantém a ordem
# física, necessária para o First-Fit percorrer do início ao fim).
particoes = []
inicio = 0
for i, tam in enumerate(TAMANHOS_PARTICOES):
    particoes.append({
        "id": i + 1,
        "inicio": inicio,     # endereço inicial da partição
        "tamanho": tam,       # tamanho fixo da partição
        "processo": None,     # nome do processo (None = livre)
        "uso": 0,             # quanto o processo realmente usa
    })
    inicio += tam

# Garante que a configuração é coerente com o tamanho da memória
assert inicio == TAMANHO_MEMORIA, "A soma das partições deve ser 1000"


def fragmentacao_total():
    """Soma o espaço desperdiçado dentro das partições ocupadas."""
    return sum(p["tamanho"] - p["uso"] for p in particoes if p["processo"])


def alocar_processo(nome, tamanho):
    """Aloca o processo na primeira partição livre que caiba (First-Fit).

    Retorna True se conseguiu alocar e False caso contrário.
    """
    print(f"\n>> Alocar {nome} ({tamanho} unidades)")

    # Validação das entradas
    if not isinstance(tamanho, int) or tamanho <= 0:
        print(f"   ERRO: tamanho inválido ({tamanho}).")
        return False
    if any(p["processo"] == nome for p in particoes):
        print(f"   ERRO: o processo {nome} já está na memória.")
        return False

    # First-Fit: percorre as partições em ordem e para na primeira que serve
    for p in particoes:
        if p["processo"] is None and p["tamanho"] >= tamanho:
            p["processo"] = nome
            p["uso"] = tamanho
            frag = p["tamanho"] - tamanho
            print(f"   OK: {nome} alocado na partição {p['id']} "
                  f"({p['tamanho']} unidades, início {p['inicio']}).")
            print(f"   Fragmentação interna nesta partição: {frag} unidades")
            print(f"   Fragmentação interna total no momento: {fragmentacao_total()} unidades")
            return True

    # Nenhuma partição serviu: explica o motivo
    maior_livre = max((p["tamanho"] for p in particoes if p["processo"] is None), default=0)
    print(f"   FALHA: nenhuma partição livre comporta {nome}.")
    print(f"   Maior partição livre: {maior_livre} unidades | "
          f"maior partição da memória: {max(TAMANHOS_PARTICOES)} unidades.")
    return False


def liberar_processo(nome):
    """Libera a partição ocupada pelo processo informado."""
    print(f"\n>> Liberar {nome}")
    for p in particoes:
        if p["processo"] == nome:
            p["processo"] = None
            p["uso"] = 0
            print(f"   OK: partição {p['id']} ({p['tamanho']} unidades) agora está livre.")
            return True
    print(f"   ERRO: o processo {nome} não está na memória.")
    return False


def exibir_memoria():
    """Mostra uma tabela com o estado atual de cada partição."""
    print("\n" + "=" * 80)
    print(f"{'Part.':<6}{'Início':>7}{'Tamanho':>9}  {'Processo':<9}{'Usado':>7}{'Frag. int.':>12}  Mapa")
    print("-" * 80)
    for p in particoes:
        if p["processo"]:
            frag = p["tamanho"] - p["uso"]
            # mapa visual: '#' = usado, '.' = desperdiçado (1 caractere = 10 unidades)
            usado = round(p["uso"] / 10)
            mapa = "#" * usado + "." * (p["tamanho"] // 10 - usado)
            print(f"{p['id']:<6}{p['inicio']:>7}{p['tamanho']:>9}  {p['processo']:<9}"
                  f"{p['uso']:>7}{frag:>12}  [{mapa}]")
        else:
            mapa = " " * (p["tamanho"] // 10)
            print(f"{p['id']:<6}{p['inicio']:>7}{p['tamanho']:>9}  {'(livre)':<9}"
                  f"{'-':>7}{'-':>12}  [{mapa}]")
    print("-" * 80)
    print(f"Fragmentação interna total: {fragmentacao_total()} unidades")
    print("=" * 80)


def main():
    print("=== Questão 2: Partições fixas com First-Fit ===")
    print(f"Memória: {TAMANHO_MEMORIA} unidades | partições: {TAMANHOS_PARTICOES}")
    exibir_memoria()

    # Sequência de testes pedida no enunciado
    alocar_processo("P1", 90)
    alocar_processo("P2", 140)
    alocar_processo("P3", 180)
    exibir_memoria()

    liberar_processo("P2")
    alocar_processo("P4", 100)   # First-Fit: cai na partição de 150 liberada por P2
    alocar_processo("P5", 350)   # deve falhar: maior partição tem 300
    exibir_memoria()

    print(f"\nRESULTADO FINAL: fragmentação interna total = {fragmentacao_total()} unidades")


if __name__ == "__main__":
    main()
