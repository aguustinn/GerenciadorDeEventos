#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <locale.h>

// ============================================================================
// ESTRUTURAS DE DADOS
// ============================================================================

typedef enum {
    ACIDENTE_TRANSITO = 1,
    FALHA_SEMAFORO,
    QUEDA_ENERGIA,
    ALAGAMENTO,
    INCENDIO
} TipoEvento;

typedef enum {
    ATIVO = 1,
    RESOLVIDO
} StatusEvento;

typedef struct {
    int dia, mes, ano;
    int hora, minuto;
} Timestamp;

// Estrutura do Nó da Árvore AVL
typedef struct AVLNode {
    int id;                     // Chave Primária
    TipoEvento tipo;
    int severidade;             // 1 a 5
    Timestamp dataHora;
    char regiao[50];
    StatusEvento status;
    
    int altura;                 // Altura do nó para cálculo do fator de balanceamento
    struct AVLNode *esq;
    struct AVLNode *dir;
} AVLNode;

// Variável global para métrica de rotações
int totalRotacoes = 0;

// ============================================================================
// FUNÇÕES AUXILIARES E MÉTRICAS DA ÁRVORE
// ============================================================================

int max(int a, int b) {
    return (a > b) ? a : b;
}

int getAltura(AVLNode *n) {
    if (n == NULL) return 0;
    return n->altura;
}

int getFatorBalanceamento(AVLNode *n) {
    if (n == NULL) return 0;
    return getAltura(n->esq) - getAltura(n->dir);
}

// ============================================================================
// ROTAÇÕES DA ÁRVORE AVL
// ============================================================================

AVLNode *rotacaoDireita(AVLNode *y) {
    AVLNode *x = y->esq;
    AVLNode *T2 = x->dir;

    x->dir = y;
    y->esq = T2;

    y->altura = max(getAltura(y->esq), getAltura(y->dir)) + 1;
    x->altura = max(getAltura(x->esq), getAltura(x->dir)) + 1;

    totalRotacoes++;
    return x;
}

AVLNode *rotacaoEsquerda(AVLNode *x) {
    AVLNode *y = x->dir;
    AVLNode *T2 = y->esq;

    y->esq = x;
    x->dir = T2;

    x->altura = max(getAltura(x->esq), getAltura(x->dir)) + 1;
    y->altura = max(getAltura(y->esq), getAltura(y->dir)) + 1;

    totalRotacoes++;
    return y;
}

// ============================================================================
// INSERÇÃO
// ============================================================================

AVLNode* criarNo(int id, TipoEvento tipo, int severidade, Timestamp dt, const char* regiao) {
    AVLNode* novo = (AVLNode*)malloc(sizeof(AVLNode));
    novo->id = id;
    novo->tipo = tipo;
    novo->severidade = severidade;
    novo->dataHora = dt;
    strncpy(novo->regiao, regiao, 49);
    novo->regiao[49] = '\0';
    novo->status = ATIVO; // Todo evento começa ativo
    novo->altura = 1;
    novo->esq = NULL;
    novo->dir = NULL;
    return novo;
}

AVLNode* inserir(AVLNode* raiz, int id, TipoEvento tipo, int severidade, Timestamp dt, const char* regiao, bool* inseriu) {
    
    if (raiz == NULL) {
        *inseriu = true;
        return criarNo(id, tipo, severidade, dt, regiao);
    }

    if (id < raiz->id)
        raiz->esq = inserir(raiz->esq, id, tipo, severidade, dt, regiao, inseriu);
    else if (id > raiz->id)
        raiz->dir = inserir(raiz->dir, id, tipo, severidade, dt, regiao, inseriu);
    else {
        // ID já existe
        *inseriu = false;
        return raiz;
    }

    //  Atualiza altura
    raiz->altura = 1 + max(getAltura(raiz->esq), getAltura(raiz->dir));

    //  Verifica balanceamento
    int fb = getFatorBalanceamento(raiz);

    // Rotação Simples à Direita 
    if (fb > 1 && id < raiz->esq->id)
        return rotacaoDireita(raiz);

    // Rotação Simples à Esquerda 
    if (fb < -1 && id > raiz->dir->id)
        return rotacaoEsquerda(raiz);

    // Rotação Dupla Esquerda-Direita 
    if (fb > 1 && id > raiz->esq->id) {
        raiz->esq = rotacaoEsquerda(raiz->esq);
        return rotacaoDireita(raiz);
    }

    // Rotação Dupla Direita-Esquerda 
    if (fb < -1 && id < raiz->dir->id) {
        raiz->dir = rotacaoDireita(raiz->dir);
        return rotacaoEsquerda(raiz);
    }

    return raiz;
}

// ============================================================================
// BUSCA E ATUALIZAÇÃO
// ============================================================================

AVLNode* buscar(AVLNode* raiz, int id) {
    if (raiz == NULL || raiz->id == id)
        return raiz;
    if (id < raiz->id)
        return buscar(raiz->esq, id);
    return buscar(raiz->dir, id);
}

// ============================================================================
// REMOÇÃO
// ============================================================================

AVLNode* noMinimo(AVLNode* no) {
    AVLNode* atual = no;
    while (atual->esq != NULL)
        atual = atual->esq;
    return atual;
}

AVLNode* remover(AVLNode* raiz, int id, bool* removido) {
    if (raiz == NULL) return raiz;

    if (id < raiz->id)
        raiz->esq = remover(raiz->esq, id, removido);
    else if (id > raiz->id)
        raiz->dir = remover(raiz->dir, id, removido);
    else {
        // Encontrou o nó. Verifica se pode ser removido (Apenas RESOLVIDO)
        if (raiz->status == ATIVO) {
            printf("\n[ERRO] O evento %d ainda esta ATIVO. Nao pode ser removido.\n", id);
            *removido = false;
            return raiz;
        }

        *removido = true;

        if ((raiz->esq == NULL) || (raiz->dir == NULL)) {
            AVLNode *temp = raiz->esq ? raiz->esq : raiz->dir;
            if (temp == NULL) {
                temp = raiz;
                raiz = NULL;
            } else {
                *raiz = *temp;
            }
            free(temp);
        } else {
            AVLNode* temp = noMinimo(raiz->dir);
            // Copia dados
            raiz->id = temp->id;
            raiz->tipo = temp->tipo;
            raiz->severidade = temp->severidade;
            raiz->dataHora = temp->dataHora;
            strcpy(raiz->regiao, temp->regiao);
            raiz->status = temp->status;
            // Remove o sucessor
            raiz->dir = remover(raiz->dir, temp->id, removido);
        }
    }

    if (raiz == NULL) return raiz;

    raiz->altura = 1 + max(getAltura(raiz->esq), getAltura(raiz->dir));
    int fb = getFatorBalanceamento(raiz);

    if (fb > 1 && getFatorBalanceamento(raiz->esq) >= 0)
        return rotacaoDireita(raiz);
    if (fb > 1 && getFatorBalanceamento(raiz->esq) < 0) {
        raiz->esq = rotacaoEsquerda(raiz->esq);
        return rotacaoDireita(raiz);
    }
    if (fb < -1 && getFatorBalanceamento(raiz->dir) <= 0)
        return rotacaoEsquerda(raiz);
    if (fb < -1 && getFatorBalanceamento(raiz->dir) > 0) {
        raiz->dir = rotacaoDireita(raiz->dir);
        return rotacaoEsquerda(raiz);
    }

    return raiz;
}

// ============================================================================
// UTILITÁRIOS DE IMPRESSÃO E CONSULTAS AVANÇADAS
// ============================================================================

const char* stringTipoEvento(TipoEvento tipo) {
    switch(tipo) {
        case ACIDENTE_TRANSITO: return "Acidente de Transito";
        case FALHA_SEMAFORO: return "Falha em Semaforo";
        case QUEDA_ENERGIA: return "Queda de Energia";
        case ALAGAMENTO: return "Alagamento";
        case INCENDIO: return "Incendio";
        default: return "Desconhecido";
    }
}

void imprimirEvento(AVLNode* n) {
    printf("ID: %04d | Tipo: %-20s | Sev: %d | Regiao: %-15s | Status: %s | Data: %02d/%02d/%04d %02d:%02d\n",
           n->id, stringTipoEvento(n->tipo), n->severidade, n->regiao,
           (n->status == ATIVO ? "ATIVO" : "RESOLVIDO"),
           n->dataHora.dia, n->dataHora.mes, n->dataHora.ano, n->dataHora.hora, n->dataHora.minuto);
}

// Consulta: Por intervalo de severidade 
void listarPorSeveridade(AVLNode* raiz, int min, int max) {
    if (raiz == NULL) return;
    listarPorSeveridade(raiz->esq, min, max);
    if (raiz->status == ATIVO && raiz->severidade >= min && raiz->severidade <= max) {
        imprimirEvento(raiz);
    }
    listarPorSeveridade(raiz->dir, min, max);
}

// Consulta: Por região 
void listarPorRegiao(AVLNode* raiz, const char* regiao) {
    if (raiz == NULL) return;
    listarPorRegiao(raiz->esq, regiao);
    if (raiz->status == ATIVO && strcmp(raiz->regiao, regiao) == 0) {
        imprimirEvento(raiz);
    }
    listarPorRegiao(raiz->dir, regiao);
}

// Consulta: Intervalo de IDs 
void listarPorIntervaloID(AVLNode* raiz, int idMin, int idMax) {
    if (raiz == NULL) return;
    
    // Se ID atual for maior que o mínimo, verifica à esquerda
    if (raiz->id > idMin) listarPorIntervaloID(raiz->esq, idMin, idMax);
    
    // Se estiver no intervalo, imprime
    if (raiz->id >= idMin && raiz->id <= idMax) imprimirEvento(raiz);
    
    // Se ID atual for menor que o máximo, verifica à direita
    if (raiz->id < idMax) listarPorIntervaloID(raiz->dir, idMin, idMax);
}

// ============================================================================
// MÉTRICAS
// ============================================================================

void calcularMetricas(AVLNode* raiz, int* totalNos, int* ativos, int* somaFatores) {
    if (raiz == NULL) return;
    (*totalNos)++;
    if (raiz->status == ATIVO) (*ativos)++;
    (*somaFatores) += getFatorBalanceamento(raiz);
    
    calcularMetricas(raiz->esq, totalNos, ativos, somaFatores);
    calcularMetricas(raiz->dir, totalNos, ativos, somaFatores);
}

void exibirMetricas(AVLNode* raiz) {
    int totalNos = 0, ativos = 0, somaFatores = 0;
    calcularMetricas(raiz, &totalNos, &ativos, &somaFatores);
    
    float mediaFb = (totalNos > 0) ? (float)somaFatores / totalNos : 0.0;
    
    printf("\n=== MÉTRICAS DA ARVORE ===\n");
    printf("1. Altura total da árvore: %d\n", getAltura(raiz));
    printf("2. Numero total de nós: %d\n", totalNos);
    printf("3. Número de eventos ativos: %d\n", ativos);
    printf("4. Fator de balanceamento médio: %.2f\n", mediaFb);
    printf("5. Rotações realizadas (histórico): %d\n", totalRotacoes);
}

// ============================================================================
// MENU E INTERFACE
// ============================================================================

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
	setlocale(LC_ALL, "Portuguese"); 
    AVLNode* raiz = NULL;
    int opcao = -1;

    while (opcao != 0) {
        printf("\n=== SISTEMA DE EVENTOS CRíTICOS SMART CITY ===\n");
        printf("1. Inserir Novo Evento\n");
        printf("2. Atualizar Evento (Status/Severidade)\n");
        printf("3. Remover Evento Resolvido\n");
        printf("4. Buscar Evento por ID\n");
        printf("5. Listar Eventos Ativos por Severidade\n");
        printf("6. Relatorio de Eventos Ativos por Regiao\n");
        printf("7. Listar Eventos por Intervalo de ID\n");
        printf("8. Exibir Métricas do Sistema\n");
        printf("0. Sair\n");
        printf("Escolha: ");
        
        if (scanf("%d", &opcao) != 1) {
            limparBuffer();
            printf("Entrada inválida!\n");
            continue;
        }

        switch (opcao) {
            case 1: { // INSERIR
                int id, tipo, sev;
                Timestamp dt;
                char regiao[50];
                printf("ID do Evento: "); scanf("%d", &id);
                printf("Tipo (1-Trânsito, 2-Semáforo, 3-Energia, 4-Alagamento, 5-Incêndio): "); scanf("%d", &tipo);
                printf("Severidade (1 a 5): "); scanf("%d", &sev);
                printf("Data (DD MM AAAA): "); scanf("%d %d %d", &dt.dia, &dt.mes, &dt.ano);
                printf("Hora (HH MM): "); scanf("%d %d", &dt.hora, &dt.minuto);
                limparBuffer();
                printf("Região: "); scanf(" %49[^\n]", regiao);

                if(sev < 1 || sev > 5) {
                    printf("[ERRO] Severidade deve ser entre 1 e 5.\n");
                    break;
                }

                bool inseriu = false;
                raiz = inserir(raiz, id, (TipoEvento)tipo, sev, dt, regiao, &inseriu);
                if (inseriu) printf("[SUCESSO] Evento cadastrado.\n");
                else printf("[ERRO] ID %d ja existe.\n", id);
                break;
            }
            case 2: { // ATUALIZAR
                int id, subop;
                printf("ID do evento para atualizar: "); scanf("%d", &id);
                AVLNode* no = buscar(raiz, id);
                if (!no) {
                    printf("[ERRO] Evento nao encontrado.\n");
                    break;
                }
                printf("1. Alterar Status para RESOLVIDO\n2. Atualizar Severidade\nEscolha: ");
                scanf("%d", &subop);
                if (subop == 1) {
                    no->status = RESOLVIDO;
                    printf("[SUCESSO] Status atualizado.\n");
                } else if (subop == 2) {
                    if (no->status == RESOLVIDO) {
                        printf("[ERRO] Não se altera severidade de evento resolvido.\n");
                    } else {
                        int novaSev;
                        printf("Nova severidade (1-5): "); scanf("%d", &novaSev);
                        if(novaSev >= 1 && novaSev <= 5) {
                            no->severidade = novaSev;
                            printf("[SUCESSO] Severidade atualizada.\n");
                        } else {
                            printf("[ERRO] Valor inválido.\n");
                        }
                    }
                }
                break;
            }
            case 3: { // REMOVER
                int id;
                printf("ID do evento a ser removido: "); scanf("%d", &id);
                bool removido = false;
                raiz = remover(raiz, id, &removido);
                if (removido) printf("[SUCESSO] Evento removido com rebalanceamento (se necessário).\n");
                break;
            }
            case 4: { // BUSCAR
                int id;
                printf("ID a buscar: "); scanf("%d", &id);
                AVLNode* no = buscar(raiz, id);
                if (no) imprimirEvento(no);
                else printf("[AVISO] Evento nao localizado.\n");
                break;
            }
            case 5: { // POR SEVERIDADE
                int min, max;
                printf("Severidade Mínima: "); scanf("%d", &min);
                printf("Severidade Máxima: "); scanf("%d", &max);
                printf("\n--- EVENTOS ATIVOS SEVERIDADE [%d - %d] ---\n", min, max);
                listarPorSeveridade(raiz, min, max);
                break;
            }
            case 6: { // RELATORIO REGIAO
                char regiao[50];
                limparBuffer();
                printf("Digite a regiao: "); scanf(" %49[^\n]", regiao);
                printf("\n--- EVENTOS ATIVOS NA REGIAO: %s ---\n", regiao);
                listarPorRegiao(raiz, regiao);
                break;
            }
            case 7: { // INTERVALO ID
                int min, max;
                printf("ID Mínima: "); scanf("%d", &min);
                printf("ID Máxima: "); scanf("%d", &max);
                printf("\n--- EVENTOS NO INTERVALO DE ID [%d - %d] ---\n", min, max);
                listarPorIntervaloID(raiz, min, max);
                break;
            }
            case 8: { // METRICAS
                exibirMetricas(raiz);
                break;
            }
            case 0:
                printf("Encerrando o sistema...\n");
                break;
            default:
                printf("Opção inválida.\n");
        }
    }

    return 0;
}
