/*
 * Lista de Exercicios II - Exercicio 1.e
 * Processo 0 distribui um vetor de tamanho N entre todos os processos da aplicacao.
 *
 * Compilar: mpicc lista2_1e.c -o lista2_1e
 * Executar: mpirun -np 4 ./lista2_1e
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int N = 12; /* tamanho do vetor - ajuste para ser multiplo do numero de processos */
    int *vetor = NULL;
    int *sub_vetor;
    int elementos_por_processo;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (N % size != 0) {
        if (rank == 0)
            fprintf(stderr, "N (%d) deve ser multiplo do numero de processos (%d).\n", N, size);
        MPI_Finalize();
        return 1;
    }

    elementos_por_processo = N / size;
    sub_vetor = (int *) malloc(elementos_por_processo * sizeof(int));

    if (rank == 0) {
        vetor = (int *) malloc(N * sizeof(int));
        printf("Vetor original: ");
        for (int i = 0; i < N; i++) {
            vetor[i] = i + 1;
            printf("%d ", vetor[i]);
        }
        printf("\n");
    }

    /* Processo 0 distribui o vetor entre todos os processos */
    MPI_Scatter(vetor, elementos_por_processo, MPI_INT,
                sub_vetor, elementos_por_processo, MPI_INT,
                0, MPI_COMM_WORLD);

    printf("Processo %d recebeu: ", rank);
    for (int i = 0; i < elementos_por_processo; i++)
        printf("%d ", sub_vetor[i]);
    printf("\n");

    free(sub_vetor);
    if (rank == 0) free(vetor);

    MPI_Finalize();
    return 0;
}
