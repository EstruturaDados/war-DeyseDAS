// ============================================================================
//         PROJETO WAR ESTRUTURADO - DESAFIO DE CÓDIGO
// ============================================================================
//        
// ============================================================================
//
// OBJETIVOS:
// - Modularizar completamente o código em funções especializadas.
// - Implementar um sistema de missões para um jogador.
// - Criar uma função para verificar se a missão foi cumprida.
// - Utilizar passagem por referência (ponteiros) para modificar dados e
//   passagem por valor/referência constante (const) para apenas ler.
// - Foco em: Design de software, modularização, const correctness, lógica de jogo.
//
// ============================================================================

// Inclusão das bibliotecas padrão necessárias para entrada/saída, alocação de memória, manipulação de strings e tempo.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

// --- Constantes Globais ---
// Definem valores fixos para o número de territórios, missões e tamanho máximo de strings, facilitando a manutenção.
#define MAX_TERRITORIOS 5
#define TAM_NOME 40
#define TAM_COR 20
#define QTD_MISSOES 2

// --- Estrutura de Dados ---
// Define a estrutura para um território, contendo seu nome, a cor do exército que o domina e o número de tropas.
typedef struct {
    char nome[TAM_NOME];
    char cor[TAM_COR];
    int tropas;
} Territorio;

// --- Protótipos das Funções ---
// Declarações antecipadas de todas as funções que serão usadas no programa, organizadas por categoria.
// Funções de setup e gerenciamento de memória:
Territorio *alocarMapa(int n);
void inicializarTerritorios(Territorio *mapa, int n);
void liberarMemoria(Territorio *mapa);

// Funções de interface com o usuário:
void exibirMenuPrincipal(void);
void exibirMapa(const Territorio *mapa, int n);
void exibirMissao(int idMissao);
void limparBufferEntrada(void);

// Funções de lógica principal do jogo:
void faseDeAtaque(Territorio *mapa, int n);
void simularAtaque(Territorio *atacante, Territorio *defensor);
int sortearMissao(void);
int verificarVitoria(const Territorio *mapa, int n, int idMissao, const char *corJogador);

// Função utilitária:

// --- Função Principal (main) ---
// Função principal que orquestra o fluxo do jogo, chamando as outras funções em ordem.
int main() {
    // 1. Configuração Inicial (Setup):
    // - Define o locale para português.
    // - Inicializa a semente para geração de números aleatórios com base no tempo atual.
    // - Aloca a memória para o mapa do mundo e verifica se a alocação foi bem-sucedida.
    // - Preenche os territórios com seus dados iniciais (tropas, donos, etc.).
    // - Define a cor do jogador e sorteia sua missão secreta.
    setlocale(LC_ALL, "Portuguese");
    srand(time(NULL));

    Territorio *mapa = alocarMapa(MAX_TERRITORIOS);
    if (mapa == NULL) {
        printf("Erro: falha ao alocar memória para o mapa.\n");
        return 1;
    }

    inicializarTerritorios(mapa, MAX_TERRITORIOS);

    const char corJogador[] = "Azul";
    int missaoID = sortearMissao();
    int opcao, venceu = 0;

    // 2. Laço Principal do Jogo (Game Loop):
    // - Roda em um loop 'do-while' que continua até o jogador sair (opção 0) ou vencer.
    // - A cada iteração, exibe o mapa, a missão e o menu de ações.
    // - Lê a escolha do jogador e usa um 'switch' para chamar a função apropriada:
    //   - Opção 1: Inicia a fase de ataque.
    //   - Opção 2: Verifica se a condição de vitória foi alcançada e informa o jogador.
    //   - Opção 0: Encerra o jogo.
    // - Pausa a execução para que o jogador possa ler os resultados antes da próxima rodada.
    do {
        exibirMapa(mapa, MAX_TERRITORIOS);
        exibirMissao(missaoID);
        exibirMenuPrincipal();

        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        limparBufferEntrada();

        switch (opcao) {
            case 1:
                faseDeAtaque(mapa, MAX_TERRITORIOS);
                break;
            case 2:
                venceu = verificarVitoria(mapa, MAX_TERRITORIOS, missaoID, corJogador);
                if (venceu)
                    printf("\nConcluiu a missão\n");
                else
                    printf("\nMissão ainda não concluída\n");
                break;
            case 0:
                printf("\nSaindo do jogo...\n");
                break;
            default:
                printf("\nOpção inválida.\n");
        }

        if (venceu) break;
    } while (opcao != 0);

    // 3. Limpeza:
    // - Ao final do jogo, libera a memória alocada para o mapa para evitar vazamentos de memória.
    liberarMemoria(mapa);

    return 0;
}

// --- Implementação das Funções ---

// alocarMapa():
// Aloca dinamicamente a memória para o vetor de territórios usando calloc.
// Retorna um ponteiro para a memória alocada ou NULL em caso de falha.
Territorio *alocarMapa(int n) {
    return (Territorio *) calloc(n, sizeof(Territorio));
}

// inicializarTerritorios():
// Preenche os dados iniciais de cada território no mapa (nome, cor do exército, número de tropas).
// Esta função modifica o mapa passado por referência (ponteiro).
void inicializarTerritorios(Territorio *mapa, int n) {
    const char *nomes[] = {"Brasil", "Argentina", "Chile", "Peru", "México"};
    const char *cores[] = {"Azul", "Vermelho", "Verde", "Amarelo", "Preto"};

    for (int i = 0; i < n; i++) {
        strcpy(mapa[i].nome, nomes[i]);
        strcpy(mapa[i].cor, cores[i]);
        mapa[i].tropas = (rand() % 5) + 1; 
    }
}

// liberarMemoria():
// Libera a memória previamente alocada para o mapa usando free.
void liberarMemoria(Territorio *mapa) {
    free(mapa);
}

// exibirMenuPrincipal():
// Imprime na tela o menu de ações disponíveis para o jogador.
void exibirMenuPrincipal(void) {
    printf("\nMENU PRINCIPAL\n");
    printf("1 - Atacar\n");
    printf("2 - Verificar Missão\n");
    printf("0 - Sair\n");
}

// exibirMapa():
// Mostra o estado atual de todos os territórios no mapa, formatado como uma tabela.
// Usa 'const' para garantir que a função apenas leia os dados do mapa, sem modificá-los.
void exibirMapa(const Territorio *mapa, int n) {
    printf("\nMAPA ATUAL\n");
    for (int i = 0; i < n; i++) {
        printf("%d - %-12s | Cor: %-10s | Tropas: %d\n",
               i + 1, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
}

// exibirMissao():
// Exibe a descrição da missão atual do jogador com base no ID da missão sorteada.
void exibirMissao(int idMissao) {
    printf("\nSUA MISSÃO\n");
    if (idMissao == 0)
        printf("Destruir o exército Verde.\n");
    else
        printf("Conquistar 3 territórios.\n");
}

// faseDeAtaque():
// Gerencia a interface para a ação de ataque, solicitando ao jogador os territórios de origem e destino.
// Chama a função simularAtaque() para executar a lógica da batalha.
void faseDeAtaque(Territorio *mapa, int n) {
    int a, d;
    exibirMapa(mapa, n);
    printf("\nEscolha o território atacante (1-%d): ", n);
    scanf("%d", &a);
    printf("Escolha o território defensor (1-%d): ", n);
    scanf("%d", &d);
    limparBufferEntrada();

    if (a < 1 || a > n || d < 1 || d > n || a == d) {
        printf("Escolha inválida.\n");
        return;
    }

    simularAtaque(&mapa[a - 1], &mapa[d - 1]);
}

// simularAtaque():
// Executa a lógica de uma batalha entre dois territórios.
// Realiza validações, rola os dados, compara os resultados e atualiza o número de tropas.
// Se um território for conquistado, atualiza seu dono e move uma tropa.
void simularAtaque(Territorio *atacante, Territorio *defensor) {
    if (atacante->tropas <= 1) {
        printf("%s não tem tropas suficientes para atacar!\n", atacante->nome);
        return;
    }

    int dadoA = rand() % 6 + 1;
    int dadoD = rand() % 6 + 1;

    printf("\n %s (%s) ataca %s (%s)!\n",
           atacante->nome, atacante->cor, defensor->nome, defensor->cor);
    printf("Dado atacante: %d | Dado defensor: %d\n", dadoA, dadoD);

    if (dadoA >= dadoD) {
        defensor->tropas--;
        printf("Atacante venceu! %s perdeu 1 tropa.\n", defensor->nome);
        if (defensor->tropas <= 0) {
            printf("%s conquistou o território %s!\n",
                   atacante->nome, defensor->nome);
            defensor->tropas = 1;
            strcpy(defensor->cor, atacante->cor);
        }
    } else {
        atacante->tropas--;
        printf("Defensor venceu! %s perdeu 1 tropa.\n", atacante->nome);
    }
}

// sortearMissao():
// Sorteia e retorna um ID de missão aleatório para o jogador.
int sortearMissao(void) {
    return rand() % QTD_MISSOES;
}

// verificarVitoria():
// Verifica se o jogador cumpriu os requisitos de sua missão atual.
// Implementa a lógica para cada tipo de missão (destruir um exército ou conquistar um número de territórios).
// Retorna 1 (verdadeiro) se a missão foi cumprida, e 0 (falso) caso contrário.
int verificarVitoria(const Territorio *mapa, int n, int idMissao, const char *corJogador) {
    int conquistas = 0;
    int verdes = 0;

    for (int i = 0; i < n; i++) {
        if (strcmp(mapa[i].cor, corJogador) == 0)
            conquistas++;
        if (strcmp(mapa[i].cor, "Verde") == 0)
            verdes++;
    }

    if (idMissao == 0)
        return verdes == 0;
    else
        return conquistas >= 3;
}

// limparBufferEntrada():
// Função utilitária para limpar o buffer de entrada do teclado (stdin), evitando problemas com leituras consecutivas de scanf e getchar.
void limparBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}