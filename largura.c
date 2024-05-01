/*
 * largura.c
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

// Função para percorrer um grafo com percurso em largura
void percurso_largura(const GRAFO * g, unsigned int r, CONJ * A, CONJ * B) {
    unsigned int F[amplitude(g)];
    unsigned int i = 0,f = 0;

    esvaziar(A);
    esvaziar(B);

    F[f++] = r;
    incluir(r, A);

    while (i < f) {
    	unsigned int u = F[i++];
    	copiar(vizinhos(g, u), B);
    	suprimir(B, A);
    	agregar(A, B);

    	NO w = partida(B);
    	while (!chegada(&w)) {
    		F[f++] = alvo(&w);
    		avancar(&w);
    	}
    }
}

// Função para medir o tempo de execução médio da função verifica_clique
double medir_tempo_execucao(const GRAFO * g, int repeticoes) {
    clock_t inicio, fim;

    CONJ A = conjunto(amplitude(g));
    CONJ B = conjunto(amplitude(g));

    inicio = clock();
    for (int i = 0; i < repeticoes; i++)
    	percurso_largura(g, 0, &A, &B);
    fim = clock();
    double tempoTotal = (double)(fim - inicio) / CLOCKS_PER_SEC;

	apagar(&B);
	apagar(&A);

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

        double tempoTotal = 0.0;

        for (int i = 0; i < nsamples; i++) {
            GRAFO g = grafo_aleatorio(numVertices, dens);
            imprimir_grafo(&g);
            
            tempoTotal += medir_tempo_execucao(&g, nrep);
            apagar_grafo(&g);
        }

        tempos[currentIndex] = tempoTotal / nsamples;
        currentIndex++;
    }

    return tempos;
}

int main() {
    int from = 1024; // Quantidade inicial de vértices
    int to = MAX_VERTICES; // Quantidade final de vértices
    int by = 256; // Incremento na quantidade de vértices
    int nsamples = 10; // Quantidade de grafos por tamanho
    int nrep = 100; // Quantidade de execuções para medição de tempo de execução
    double r = 0.1; // Fração que determina tamanho do subconjunto

    // Define a semente para geração de números aleatórios
    srand(time(NULL));

    printf("Parâmetros do experimento:\nMAX_VERTICES: %d\nLOG_MAX_VERTICES: %d\nNBITS_NACO: %d\nLOG_NBITS_NACO: %d\n\n\n",
    		MAX_VERTICES,
			LOG_MAX_VERTICES,
			NBITS_NACO,
			LOG_NBITS_NACO);

    printf("Tempo médio de execução:\n");
    for(double dens = 0.25; dens <= 1; dens += 0.25) { // Densidade esperada dos grafos
    	if (dens == 1)
    		dens = 0.95;
    	double* tempos = estimar_tempo_execucao(from, to, by, nsamples, dens, nrep, r);
    	int currentIndex = 0;
    	for (int n = from; n <= to; n += by) {
    		printf("Número de vértices: %d, Densidade: %f, Tempo: %e segundos\n", n, dens, tempos[currentIndex]);
    		currentIndex++;
    	}

    	free(tempos);
    }

    return 0;
}

