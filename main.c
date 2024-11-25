#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "main.h"

#define MAX_PROCESSES 10
int tamanho_memoria_fisica = 128; // Configurável
int tamanho_pagina = 16; // Configurável
int tamanho_maximo_processo = 64; // Configurável

typedef struct {
    int id_processo;
    int tamanho_processo;
    int *tabela_paginas;
} Processo;

typedef struct {
    int numero_quadro;
    int ocupado;
    int id_processo;
} Quadro;

Quadro *memoria_fisica;
Processo processos[MAX_PROCESSES];
int contador_processos = 0;

void linha(){
    printf("---------------------------------------------------------------\n");
    return;
}

void inicializar_memoria() {
    int num_quadros = tamanho_memoria_fisica / tamanho_pagina;
    memoria_fisica = (Quadro *)malloc(num_quadros * sizeof(Quadro));
    for (int i = 0; i < num_quadros; i++) {
        memoria_fisica[i].numero_quadro = -1;
        memoria_fisica[i].ocupado = 0;
        memoria_fisica[i].id_processo = -1;
    }
}

int encontrar_quadro_livre() {
    int num_quadros = tamanho_memoria_fisica / tamanho_pagina;
    for (int i = 0; i < num_quadros; i++) {
        if (!memoria_fisica[i].ocupado) {
            return i;
        }
    }
    return -1;
}

int conta_quadros_livres(){
    int num_quadros = tamanho_memoria_fisica / tamanho_pagina;
    int quadros_livres = 0;
    for (int i = 0; i < num_quadros; i++){
        if (!memoria_fisica[i].ocupado){
            quadros_livres++;
        }
    }
    return quadros_livres;
}

void ver_memoria() {
    int num_quadros = tamanho_memoria_fisica / tamanho_pagina;
    int quadros_livres = 0;
    for (int i = 0; i < num_quadros; i++) {
        if (memoria_fisica[i].ocupado) {
            printf("Quadro %d: Ocupado pelo Processo %d\n", i, memoria_fisica[i].id_processo  );
        } else {
            printf("Quadro %d: Livre\n", i);
            quadros_livres++;
        }
    }
    printf("\n");
    printf("Memoria LIVRE: %.2f%%\n", (quadros_livres / (float)num_quadros) * 100);
    linha();
    printf("\n");
}

int verifica_ID_processo(int id_processo){
    for (int i = 0; i < contador_processos; i++){
        if (processos[i].id_processo == id_processo){
            return 1; //ID ja em uso
        }
    }
    return 0; //nao esta em uso
}

void criar_processo(int id_processo, int tamanho_processo) {
    if (tamanho_processo > tamanho_maximo_processo) {
        printf("Erro: Tamanho do processo excede o limite de %d bytes por processo.\n", tamanho_maximo_processo);
        linha();
        printf("\n");
        return;
    }

    if (verifica_ID_processo(id_processo)){
        printf("ERRO: ID do Processo %d, ja existe. Tente outro.\n", id_processo);
        linha();
        printf("\n");
        return;
    }

    int numero_paginas = (tamanho_processo + tamanho_pagina - 1) / tamanho_pagina; // numero de paginas necessecarias
    int quadros_livres = conta_quadros_livres();
    if (quadros_livres < numero_paginas){
        printf("ERRO: Sem memoria suficiente para alocar o processo\n");
        linha();
        printf("\n");
        return;
    }

    int *tabela_paginas = (int *)malloc(numero_paginas * sizeof(int));

    for (int i = 0; i < numero_paginas; i++) {
        int quadro_livre = encontrar_quadro_livre();
        memoria_fisica[quadro_livre].ocupado = 1;
        memoria_fisica[quadro_livre].id_processo = id_processo;
        tabela_paginas[i] = quadro_livre;
    }

    Processo novo_processo;
    novo_processo.id_processo = id_processo;
    novo_processo.tamanho_processo = tamanho_processo;
    novo_processo.tabela_paginas = tabela_paginas;

    processos[contador_processos++] = novo_processo;
    printf("Processo %d criado com SUCESSO.\n", id_processo);
    linha();
    printf("\n");

}

void modifica_configuracao(){
    printf("Tamanho da MEMORIA FISICA atual: %d bytes\n", tamanho_memoria_fisica);
    printf("Digite o novo tamanho da MEMORIA FISICA (multiplo de 2): ");
    scanf("%d", &tamanho_memoria_fisica);
    printf("\n");

    printf("Tamanho da PAGINA (QUADRO) atual: %d bytes\n", tamanho_pagina);
    printf("Digite o novo tamanho da PAGINA (QUADRO): ");
    scanf("%d", &tamanho_pagina);
    printf("\n");

    printf("Tamanho MAXIMO DE UM PROCESSO atual: %d bytes\n", tamanho_maximo_processo);
    printf("Digite o novo tamanho MAXIMO DE UM PROCESSO: ");
    scanf("%d", &tamanho_maximo_processo);
    printf("\n");
    linha();
    free(memoria_fisica);
    inicializar_memoria();
}

void ver_tabela_paginas(int id_processo) {
    for (int i = 0; i < contador_processos; i++) {
        if (processos[i].id_processo == id_processo) {
            printf("ID do Processo: %d\n", id_processo);
            printf("Tamanho do Processo: %d bytes\n", processos[i].tamanho_processo);
            int numero_paginas = (processos[i].tamanho_processo + tamanho_pagina - 1) / tamanho_pagina;
            for (int j = 0; j < numero_paginas; j++) {
                printf("Pagina %d -> Quadro %d\n", j, processos[i].tabela_paginas[j]);
            }
            linha();
            printf("\n");
            return;
        }
    }
    printf("ERRO: Processo não encontrado.\n");
    linha();
    printf("\n");
}

int main() {
    inicializar_memoria();
    srand(time(0));

    while (1) {
        int opcao;
        printf("1. Ver Memoria\n");
        printf("2. Criar Processo (tamanho maximo = %d bytes)\n", tamanho_maximo_processo);
        printf("3. Ver Tabela de Paginas\n");
        printf("4. Modificar Configuração Inicial\n");
        printf("5. Sair\n");
        printf("\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        printf("\n");
        linha();

        if (opcao == 1) {
            ver_memoria();
        } else if (opcao == 2) {
            int id_processo, tamanho_processo;
            printf("Digite o ID do processo: ");
            scanf("%d", &id_processo);
            printf("Digite o tamanho do processo: ");
            scanf("%d", &tamanho_processo);
            criar_processo(id_processo, tamanho_processo);
        } else if (opcao == 3) {
            int id_processo;
            printf("Digite o ID do processo: ");
            scanf("%d", &id_processo);
            ver_tabela_paginas(id_processo);
        } else if (opcao == 4) {
            modifica_configuracao();
        } else if (opcao == 5) {
            break;
        } else {
            printf("Opcao invalida, TENTE NOVAMENTE.\n");
            linha();
            printf("\n");
        }
    }

    free(memoria_fisica);

    return 0;
}
