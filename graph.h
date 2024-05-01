/*
 * graph.h
 *
 *  Created on: 15 de abr. de 2024
 *      Author: correa
 */

#ifndef SRC_CHATGPT_GRAPH_H_
#define SRC_CHATGPT_GRAPH_H_

#define NACO unsigned long long   // tipo do NACO

#define bool NACO
#define true (NACO) 1
#define false (NACO) 0

#define MAX_VERTICES 8192
#define LOG_MAX_VERTICES 13
#define NBYTES_NACO 8       // tamanho do NACO: sizeof(unsigned long long) = 8
#define LOG_NBYTES_NACO 3
#define NBITS_NACO 64
#define LOG_NBITS_NACO 6

#define N_NACOS(n) (((n >> LOG_NBITS_NACO) << LOG_NBITS_NACO) == n ? (n >> LOG_NBITS_NACO) : ((n >> LOG_NBITS_NACO)+1))
#define IDX_NACO(v) (v >> LOG_NBITS_NACO)
#define IDX_EM_NACO(v) (v - (IDX_NACO(v) << LOG_NBITS_NACO))

#define NACO_DE(a,v) a[IDX_NACO(v)]

// Tipo CONJ
typedef struct {
    unsigned long long conj[MAX_VERTICES / NBITS_NACO];
} CONJ;

// Remove todos os elementos de um conjunto especificado, deixando-o vazio.
static inline void esvaziar(CONJ * R) {
    memset(R->conj, 0, N_NACOS(MAX_VERTICES) * NBYTES_NACO);
}

// Cria um novo conjunto com a amplitude indicada.
// Retorna o conjunto criado.
CONJ conjunto(int ampl) {
    CONJ R;
    esvaziar(&R);
    return R;
}

// Apaga um conjunto da memória, liberando a memória por ele ocupada
void apagar(CONJ * R) {
}

// Retorna a cardinalidade de um conjunto
static inline unsigned int cardinalidade(const CONJ * R) {
    unsigned int count = 0;
    for (int i = 0; i < N_NACOS(MAX_VERTICES); i++) {
        count += __builtin_popcountll(R->conj[i]);
    }
    return count;
}

// Replica um conjunto especificado em um novo conjunto.
// O conjunto especificado não pode ser alterado.
// Retorna o conjunto criado.

CONJ replicar(const CONJ * R) {
    CONJ S;
    memcpy(S.conj, R->conj, N_NACOS(MAX_VERTICES) * NBYTES_NACO);
    return S;
}

// Copia os elementos de um conjunto de origem para um conjunto de destino.
// O conjunto de origem não pode ser alterado.
static inline void copiar(const CONJ * R, CONJ * S) {
    memcpy(S->conj, R->conj, N_NACOS(MAX_VERTICES) * NBYTES_NACO);
}

// Verifica se um valor pertence a um conjunto.
static inline bool pertence(unsigned int i, const CONJ * R) {
    return (R->conj[IDX_NACO(i)] >> IDX_EM_NACO(i)) & 1;
}

// Incluir um elemento em um conjunto.
static inline void incluir(unsigned int i, CONJ * R) {
    R->conj[IDX_NACO(i)] |= (1ULL << IDX_EM_NACO(i));
}

// Retirar um elemento em um conjunto.
static inline void retirar(unsigned int i, CONJ * R) {
    R->conj[IDX_NACO(i)] &= ~(1ULL << IDX_EM_NACO(i));
}

// Operações sobre conjuntos

// Verifica se um conjunto R é subconjunto de outro conjunto S.
// Ambos os conjunto devem ter a mesma amplitude.
static inline bool subconj(const CONJ * R, const CONJ * S) {
    for (int i = 0; i < N_NACOS(MAX_VERTICES); i++) {
        if ((R->conj[i] & S->conj[i]) != R->conj[i]) {
            return false;
        }
    }
    return true;
}

// Atribui a um conjunto R a sua interseção com outro conjunto S.
// Ambos os conjuntos devem ter a mesma amplitude.
// O conjunto S não pode ser alterado.
static inline void manter(CONJ * R, const CONJ * S) {
    for (int i = 0; i < N_NACOS(MAX_VERTICES); i++) {
        R->conj[i] &= S->conj[i];
    }
}

// Atribui a um conjunto R a sua diferença para outro conjunto S.
// Ambos os conjuntos devem ter a mesma amplitude.
// O conjunto S não pode ser alterado.
static inline void suprimir(CONJ * R, const CONJ * S) {
    for (int i = 0; i < N_NACOS(MAX_VERTICES); i++) {
        R->conj[i] &= ~S->conj[i];
    }
}

// Atribui a um conjunto R a sua união com outro conjunto S.
// Ambos os conjuntos devem ter a mesma amplitude.
// O conjunto S não pode ser alterado.
static inline void agregar(CONJ * R, const CONJ * S) {
    for (int i = 0; i < N_NACOS(MAX_VERTICES); i++) {
        R->conj[i] |= S->conj[i];
    }
}



// Tipo Nó utilizado para enumeração de CONJ
typedef struct {
    int idx;
    int bit_idx;
} NO;

// Cria e retorna um novo nó cujo alvo vale -1.
static inline NO no(CONJ * R) {
    NO n = {0, 0};
    return n;
}

// Retorna o alvo de um nó.
// O nó especificado não pode ser modificado.
static inline unsigned int alvo(const NO * r) {
    return r->idx * NBITS_NACO + r->bit_idx;
}

// Enumeração de conjuntos
static inline NO partida(CONJ * const R) {
    NO n = {0, 0};
    return n;
}

static inline bool chegada(NO * r) {
    return r->idx >= N_NACOS(MAX_VERTICES);
}

static inline NO * avancar(NO * r) {
    r->bit_idx++;
    if (r->bit_idx >= NBITS_NACO) {
        r->idx++;
        r->bit_idx = 0;
    }
    return r;
}

static inline void imprimir(CONJ * R) {
    printf("Conjunto com %d elementos.\n{", cardinalidade(R));
    NO r = partida(R);
    while (!chegada(&r)) {
        if (pertence(alvo(&r), R)) {
            printf("%d,", alvo(&r));
        }
        avancar(&r);
    }
    printf("}\n");
}

static inline void retirar_do_conjunto(NO * r) {
}

// Tipo GRAFO
// A qtd de vértices não pode ser alterada.
typedef struct {
	CONJ * const viz;
	int nvert;
} GRAFO;

// Cria e retorna um grafo sem arestas com a qtd de vértices especificada.
GRAFO grafo(int n) {
	CONJ * a = (CONJ *) calloc(n, sizeof(CONJ));
	for (int i = 0; i < n; i++) {
		CONJ c = conjunto(n);
		memcpy(a+i, &c, sizeof(CONJ));
	}
	GRAFO g = {a, n};
	return g;
}

// Apaga um grafo da memória, liberando a memória por ele ocupada
void apagar_grafo(GRAFO * g) {
	for (int i = 0; i < g->nvert; i++)
		apagar(&g->viz[i]);
	free(g->viz);
	g->nvert = 0;
}

// Operações sobre grafos

// Retorna a cardinalidade de um conjunto
static inline unsigned int amplitude(const GRAFO * g) {
	return g->nvert;
}

// Retorna o conjunto de vizinhos de um vértice e um grafo.
// O conjunto retornado não pode ser modificado.
static inline const CONJ * vizinhos(const GRAFO * g, unsigned int v) {
	return &g->viz[v];
}

// Inclui uma aresta definida pelos seus extremos em um grafo.
// Não deve ser usada com u=v
static inline void incluir_aresta(unsigned int u, unsigned int v, GRAFO * g) {
	incluir(v, &g->viz[u]);
	incluir(u, &g->viz[v]);
}

// Verifica se uma aresta definida pelos seus extremos está presente em um grafo.
static inline bool existe_aresta(unsigned int u, unsigned int v, GRAFO * g) {
	return pertence(v, vizinhos(g, u));
}

// Retorna qtd de arestas no grafo
unsigned long n_arestas(const GRAFO * g) {
	unsigned long m = 0;
	for (int i = 0; i < g->nvert; i++)
		m += cardinalidade(vizinhos(g, i));
	return m >> 1;
}

void imprimir_grafo(const GRAFO * g) {
	printf("Grafo com %d vértices e %ld arestas.\n", g->nvert, n_arestas(g));
}


// Função para gerar um grafo aleatório com base nos parâmetros especificados
GRAFO grafo_aleatorio(int numVertices, double densidade) {
	GRAFO g = grafo(numVertices);

	// Calcula o número esperado de arestas
    int numArestas = (int)(densidade * numVertices * (numVertices - 1) / 2);

    // Gera arestas aleatórias até atingir o número esperado
    int count = 0;
    while (count < numArestas) {
        int v1 = rand() % numVertices; // Vértice inicial aleatório
        int v2 = rand() % numVertices; // Vértice final aleatório

        if (v1 != v2 && !existe_aresta(v1, v2, &g)) {
            // Adiciona a aresta ao grafo
        	incluir_aresta(v1, v2, &g);
            count++;
        }
    }

    return g;
}

#endif /* SRC_CHATGPT_GRAPH_H_ */
