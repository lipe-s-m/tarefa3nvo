/*
 * clique_completo.c
 *
 *  Created on: 25 de março de 2024
 *      Author: correa
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "graph.h"

// Função para verificar se um subconjunto define uma clique no grafo
bool verifica_clique(const GRAFO * g, const CONJ * subset) {
    CONJ R = replicar(subset);
    NO r = partida(&R);
    while (!chegada(&r)) {
    	retirar_do_conjunto(&r);
    	if (!subconj(&R, vizinhos(g, alvo(&r)))) {
    		apagar(&R);
    		return false;
    	}
    	avancar(&r);
    }
	apagar(&R);
    return true;
}

// Função para construir uma clique com limite de tamanho
void clique(const GRAFO * g, CONJ * subset, int lim) {
	if (lim == 0)
		return;

    int vertex = rand() % g->nvert; // Escolhe um vértice aleatório
    incluir(vertex, subset);
    int c = 1;

    CONJ R = replicar(vizinhos(g, vertex));
    NO r = partida(&R);
    while (c < lim && !chegada(&r)) {
        incluir(alvo(&r), subset);
        c++;
        manter(&R, vizinhos(g, alvo(&r)));
    	avancar(&r);
    }
	apagar(&R);
}

// Função para medir o tempo de execução médio da função verifica_clique
double medir_tempo_execucao(const GRAFO * g, const CONJ * subset, int repeticoes) {
    clock_t inicio, fim;

    inicio = clock();
    for (int i = 0; i < repeticoes; i++)
        verifica_clique(g, subset);
    fim = clock();
    double tempoTotal = (double)(fim - inicio) / CLOCKS_PER_SEC;

    return tempoTotal / repeticoes;
}

// Função para estimar o tempo de execução da função verifica_clique para diferentes tamanhos de grafos
double* estimar_tempo_execucao(int from, int to, int by, int nsamples, double dens, int nrep, double r) {
    int numVertices, subsetSize;
    double* tempos = malloc(((to - from) / by + 1) * sizeof(double));

    printf("Estimar tempo médio de execução\nfrom: %d\nto: %d\nby: %d\nnsamples: %d\ndens: %f\nnrep: %d\nr: %f\n",
    		from, to, by, nsamples, dens, nrep, r);
    fflush(NULL);

    int currentIndex = 0;
    for (int n = from; n <= to; n += by) {
        numVertices = n;
        subsetSize = n * r;

        CONJ subset = conjunto(numVertices);

        double tempoTotal = 0.0;

        for (int i = 0; i < nsamples; i++) {
            GRAFO g = grafo_aleatorio(numVertices, dens);
            imprimir_grafo(&g);

            esvaziar(&subset);
            clique(&g, &subset, subsetSize);
            
            tempoTotal += medir_tempo_execucao(&g, &subset, nrep);
            apagar_grafo(&g);
        }

        tempos[currentIndex] = tempoTotal / nsamples;
        currentIndex++;
        apagar(&subset);
    }

    return tempos;
}

int main() {
    int from = 1024; // Quantidade inicial de vértices
    int to = MAX_VERTICES; // Quantidade final de vértices
    int by = 256; // Incremento na quantidade de vértices
    int nsamples = 10; // Quantidade de grafos por tamanho
    double dens = 0.99; // Densidade esperada dos grafos
    int nrep = 100; // Quantidade de execuções para medição de tempo de execução
    double r = 0.1; // Fração que determina tamanho do subconjunto

    // Define a semente para geração de números aleatórios
    srand(time(NULL));

    printf("Parâmetros do experimento:\nMAX_VERTICES: %d\nLOG_MAX_VERTICES: %d\nNBITS_NACO: %d\nLOG_NBITS_NACO: %d\n\n\n",
    		MAX_VERTICES,
			LOG_MAX_VERTICES,
			NBITS_NACO,
			LOG_NBITS_NACO);

    double* tempos = estimar_tempo_execucao(from, to, by, nsamples, dens, nrep, r);

    printf("Tempo médio de execução:\n");
    int currentIndex = 0;
    for (int n = from; n <= to; n += by) {
        printf("Número de vértices: %d, Tempo: %e segundos\n", n, tempos[currentIndex]);
        currentIndex++;
    }

    free(tempos);

    return 0;
}

