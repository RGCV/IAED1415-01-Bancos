/*
 *****************************************************************************
*o***************************************************************************o*
***-------------------------------------------------------------------------***
**'             1o. projecto de IAED | 2o. semestre - 2014/2015             '**
**'                 TERMINAL DE GESTAO DE UMA REDE DE BANCOS                '**
**'    Grupo: 81045-Rui Ventura; 81338-Pedro Cerejo; 81670-Joao Oliveira    '**
***-------------------------------------------------------------------------***
*o***************************************************************************o*
 *****************************************************************************
*/

/* -- CABECALHOS -- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* -- CONSTANTES -- */
#define BANCO_BOM 1 /* Representacao numerica de um banco classificado */
#define BANCO_MAU 0 /* Representacao numerica de um banco desclassificado */
#define MAXREGS 1000 /* Numero maximo possivel de registos na rede bancaria */
#define MAXNOME 41 /* Comprimento maximo do nome de um banco (40 chrs + 1 '\0') */

/* -- ESTRUTURAS -- */
typedef struct banco {
    int referencia; /* Referencia identificativa do banco (ID, chave,..) */
    char nome[MAXNOME]; /* Nome familiar ao utilizador associado a referencia */
    short int classificacao; /* Classificacao do banco (1 = bom, 0 = mau) */
    int inP, outP; /* Numero de parceiros com credito / divida */
    int outV, outVM; /* Valor emprestado, total e a bancos desclassificados */
    int inV, inVM; /* Valor amortizado, total e a bancos desclassificados */
} Banco;

/* -- PROTOTIPOS -- */
int bancosClassificados();
int procuraReferencia(int referencia);

void adicionaBanco();
void classificaBanco();
void desclassificaBanco();
void desclassificaPior();
void listagem();
void emprestimo();
void amortiza();

/* -- GLOBAIS -- */
Banco rede[MAXREGS]; /* O vector de bancos registados na rede */
int num_bancos; /* Numero total de bancos */
int matriz[MAXREGS][MAXREGS]; /* Matriz de adjacencia (armazena as transacoes) */

/* -- MAIN -- */

/*
    - Params: ---
    - Funcao: Responsavel.
    - Devolve: (int) Sucesso / Falhanco
*/
int main() {
    int comando;
    
    while (1) {
        switch (comando = getchar()) {
            case 'a':
                adicionaBanco();
                break;
            case 'e':
                emprestimo();
                break;
            case 'K':
                desclassificaPior();
                break;
            case 'k':
                desclassificaBanco();
                break;
            case 'r':
                classificaBanco();
                break;
            case 'l':
                listagem();
                break;
            case 'p':
                amortiza();
                break;
            case 'x':
                printf("%d %d\n", num_bancos, bancosClassificados());
                return EXIT_SUCCESS;
            default:
                printf("[ERRO] Comando nao reconhecido (%c)\n", comando);
                break;
        }
        
        getchar();
    }
    
    return EXIT_FAILURE;
}

/* -- FUNCOES -- */

/*
    - Params: ---
    - Funcao: Percorrer todos os bancos registados na rede bancaria e calcular
    o numero de bancos classificados.
    - Devolve: (int) Numero de bancos classificados
*/
int bancosClassificados() {
    int i, classificados = 0;
    for (i = 0; i < num_bancos; i++)
        if (rede[i].classificacao == BANCO_BOM) classificados++;
    return classificados;
}

/*
    - Params: (int) referencia
    - Funcao: Percorrer todos os bancos registados na rede bancaria ate encon-
    trar o banco com a referencia indicada.
    - Devolve: (int) Indice do banco com a referencia indicada.
*/
int procuraReferencia(int referencia) {
    int i;
    for (i = 0; i < num_bancos; i++)
        if (rede[i].referencia == referencia)
            return i;
    return -1;
}

/*
    - Params: ---
    - Funcao: Adicionar um banco a rede de bancos, verificando previamente se
    e possivel regista-lo, validando a capacidade da rede, comparando ainda
    a referencia e o nome com os bancos registados (evitar duplicados).
    - Devolve: ---
*/
void adicionaBanco() {
    char nome[MAXNOME];
    short int classificacao;
    int i, referencia;
    
    if (num_bancos < MAXREGS) {
        scanf("%s%hd%d", nome, &classificacao, &referencia);
        for (i = 0; i < num_bancos; i++) {
            if (strcmp(rede[i].nome, nome) == 0) {
                printf("[ERRO] Banco com o mesmo nome encontrado.\n");
                break;
            } else if (rede[i].referencia == referencia) {
                printf("[ERRO] Banco com a mesma referencia encontrado.\n");
                break;
            }
        }
        
        if (i == num_bancos) {
            strcpy(rede[num_bancos].nome, nome);
            rede[num_bancos].classificacao = classificacao;
            rede[num_bancos++].referencia = referencia;
        }
    } else
        printf("[ERRO] Numero maximo de registos atingido.\n");
}

/*
    - Params: ---
    - Funcao: Classificar um determinado banco como um banco bom, actualizando,
    se for caso disso, os valores de transacoes 'malignos' entre os seus par-
    ceiros.
    - Devolve: ---
*/
void classificaBanco() {
    int referencia, i, j;
    
    scanf("%d", &referencia);
    i = procuraReferencia(referencia);
    if (i > -1) {
        if (rede[i].classificacao != BANCO_BOM) {
            rede[i].classificacao = BANCO_BOM;
            for (j = 0; j < MAXREGS; j++){
                if (matriz[i][j] != 0)
                    rede[j].inVM -= matriz[i][j];
                if (matriz[j][i] != 0)
                    rede[j].outVM -= matriz[j][i];
        
            }    
        }
    }
    else 
       printf("[ERRO] Nao existe nenhum banco com tal referencia!\n");
}

/*
    - Params: ---
    - Funcao: Classificar um determinado banco como um banco mau, actualizando,
    se for caso disso, os valores de transacoes 'malignos' entre os seus par-
    ceiros.
    - Devolve: (int) Numero de bancos classificados
*/
void desclassificaBanco() {
    int referencia, i, j;
    
    scanf("%d", &referencia);
    i = procuraReferencia(referencia);
    if (i > -1) {
        if (rede[i].classificacao != BANCO_MAU) {
            rede[i].classificacao = BANCO_MAU;
            for (j = 0; j < MAXREGS; j++){
                if (matriz[i][j] != 0)
                    rede[j].inVM += matriz[i][j];
                if (matriz[j][i] != 0)
                    rede[j].outVM += matriz[j][i];
        
            }    
        }
    }
    else 
       printf("[ERRO] Nao existe nenhum banco com tal referencia!\n");
}

/*
    - Params: ---
    - Funcao: Classificar o 'pior' banco bom como mau, ou seja, o banco classi-
    ficado com a maior divida para com bancos maus ou desclassificados.
    - Devolve: ---
*/
void desclassificaPior() {
    int i, j, pior = -1;
    
    for (i = 0; i < num_bancos; i++) {
        if (rede[i].classificacao == BANCO_BOM &&
            rede[i].outVM >= rede[pior].outVM)
            pior = i;
    }
    
    if (pior > -1 && rede[pior].outVM > 0) {
        rede[pior].classificacao = BANCO_MAU;
        
        for (j = 0; j < num_bancos; j++){
            if (matriz[pior][j] != 0)
                rede[j].inVM += matriz[pior][j];
            if (matriz[j][pior] != 0)
                rede[j].outVM += matriz[j][pior];
    
        }
        
        printf("*%d %s %hd %d %d %d %d %d %d\n",
                   rede[pior].referencia, rede[pior].nome,
                   rede[pior].classificacao,
                   rede[pior].inP, rede[pior].outP,
                   rede[pior].outV, rede[pior].outVM,
                   rede[pior].inV, rede[pior].inVM);
    }
    printf("%d %d\n", num_bancos, bancosClassificados());
}

/*
    - Params: ---
    - Funcao: Emitir uma listagem de tres maneiras distintas. Uma listagem
    simples (nome, referencia e classificacao), uma mais detalhada (com a adicao
    de numero de parceiros com credito ou divida e valores de emprestimo e
    amortizacao totais e para com bancos maus) e uma terceira que revela um
    histograma (b n, numero de bancos b com n parceiros)
    - Devolve: ---
*/
void listagem() {
    short int modo;
    int i, j, parceiros, nBancos[MAXREGS] = {0};
    
    scanf("%hd", &modo);
    switch (modo) {
        case 0:
        case 1:
            for (i = 0; i < num_bancos ; i++) {
                printf("%d %s %hd",
                       rede[i].referencia, rede[i].nome, rede[i].classificacao);
                printf(modo ? " %d %d %d %d %d %d\n" : "\n",
                       rede[i].inP, rede[i].outP, 
                       rede[i].outV, rede[i].outVM,
                       rede[i].inV, rede[i].inVM);
            }
            break;
        case 2:
            for (i = 0; i < num_bancos; i++) {
                parceiros = 0;
                for (j = 0; j < num_bancos; j++) {
                    if (i != j && (matriz[i][j] > 0 || matriz[j][i] > 0))
                        parceiros++;
                }
                nBancos[parceiros]++;
            }
            
            for(i = 0; i < num_bancos ; i++)
                if (nBancos[i] != 0)
                    printf("%d %d\n", i, nBancos[i]);
            break;
        default:
            printf("[ERRO] Nao especificou o modo de listagem.\n"); 
            break;
    }
}

/*
    - Params: ---
    - Funcao: Realizar um emprestimo entre os bancos A e B, validando previamen-
    te o montante inserido.
    - Devolve: ---
*/
void emprestimo() {
    int refOrig, refDest, valor, i = -1, j = -1, k;

    scanf("%d%d%d", &refOrig, &refDest, &valor);
    if (valor >= 0) {
        if (valor != 0) {
            for (k = 0; k < num_bancos && (i < 0 || j < 0); k++) {
                if (rede[k].referencia == refOrig)
                    i = k;
                else if (rede[k].referencia == refDest)
                    j = k;
            }
            if (i != j && i > -1 && j > -1) {
                if (matriz[i][j] == 0) {
                    rede[i].outP++;
                    rede[j].inP++;
                }
                if (rede[i].classificacao == BANCO_MAU) 
                    rede[j].inVM += valor;
                if (rede[j].classificacao == BANCO_MAU)
                    rede[i].outVM += valor;
                rede[i].outV += valor;
                rede[j].inV += valor;
                matriz[i][j] += valor;
            } else
                printf("[ERRO] A(s) referencia(s) introduzida(s) %s!\n",
                       i == j ? "sao iguais" : "esta(o) incorreta(s)");
        }
    } else 
        printf("[ERRO] O montante introduzido nao e permitido!\n");
}

/*
    - Params: ---
    - Funcao: Realizar uma amortizacao entre os bancos A e B, validando previa-
    mente o montante inserido.
    - Devolve: ---
*/
void amortiza() {
    int refOrig, refDest, valor, i = -1, j = -1, k;

    scanf("%d%d%d", &refOrig, &refDest, &valor);
    if (valor >= 0) {
        if (valor != 0) {
            for (k = 0; k < num_bancos && (i < 0 || j < 0); k++) {
                if (rede[k].referencia == refOrig)
                    j = k;
                else if (rede[k].referencia == refDest)
                    i = k;
            }
            if (i != j && i > -1 && j > -1) {
                if (matriz[i][j] >= valor) {
                    matriz[i][j] -= valor;
                    if (matriz[i][j] == 0) {
                        rede[i].outP--;
                        rede[j].inP--;
                    }
                    if (rede[i].classificacao == BANCO_MAU)
                        rede[j].inVM -= valor;
                    if (rede[j].classificacao == BANCO_MAU)
                        rede[i].outVM -= valor;
                    rede[i].outV -= valor;
                    rede[j].inV -= valor;
                } else
                    printf("[ERRO] Valor a amortizar tem que ser inferior ou " \
                           "igual ao da divida.\n");
            } else
                printf("[ERRO] A(s) referencia(s) introduzida(s) %s!\n",
                       i == j ? "sao iguais" : "esta(o) incorreta(s)");
        }
    } else 
        printf("[ERRO] O valor introduzido nao e permitido!\n");
}