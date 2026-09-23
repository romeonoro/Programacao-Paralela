/*
 * Lista de Exercicios III - Exercicio 1
 * Somatorio dos elementos de um vetor com N elementos usando P processos,
 * com Comunicacao Ponto-a-Ponto (MPI_Send / MPI_Recv). Paralelismo de Dados.
 *
 * a. O processo Zero gera os elementos do vetor.
 * b. O processo Zero particiona o vetor entre os P processos.
 * c. Cada processo Pi executa a soma parcial de seus elementos.
 * d. O processo Um recebe os somatorios parciais, calcula e mostra o
 *    somatorio final.
 *
 * Compilar: mpicc lista3_1.c -o lista3_1
 * Executar: mpirun -np 4 ./lista3_1   (requer no minimo 2 processos)
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int N = 20; /* tamanho do vetor - ajuste conforme necessario */
    int *vetor = NULL;
    int *sub_vetor;
    int elementos_por_processo, resto;
    int soma_parcial = 0, soma_total = 0;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        if (rank == 0)
            fprintf(stderr, "Este programa requer ao menos 2 processos (0 e Um).\n");
        MPI_Finalize();
        return 1;
    }

    elementos_por_processo = N / size;
    resto = N % size;

    /* a. Processo Zero gera os elementos do vetor */
    if (rank == 0) {
        vetor = (int *) malloc(N * sizeof(int));
        printf("Vetor gerado: ");
        for (int i = 0; i < N; i++) {
            vetor[i] = i + 1;
            printf("%d ", vetor[i]);
        }
        printf("\n");
    }

    /* b. Processo Zero particiona o vetor e envia a cada processo via
          comunicacao ponto-a-ponto */
    if (rank == 0) {
        int offset = elementos_por_processo + resto; /* processo 0 fica com o resto extra */
        sub_vetor = (int *) malloc(offset * sizeof(int));
        for (int i = 0; i < offset; i++)
            sub_vetor[i] = vetor[i];

        int pos = offset;
        for (int dest = 1; dest < size; dest++) {
            MPI_Send(&vetor[pos], elementos_por_processo, MPI_INT, dest, 0, MPI_COMM_WORLD);
            pos += elementos_por_processo;
        }

        /* c. Processo Zero calcula sua propria soma parcial */
        for (int i = 0; i < offset; i++)
            soma_parcial += sub_vetor[i];

        free(sub_vetor);
        free(vetor);
    } else {
        sub_vetor = (int *) malloc(elementos_por_processo * sizeof(int));
        MPI_Recv(sub_vetor, elementos_por_processo, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        /* c. Cada processo Pi calcula sua soma parcial */
        for (int i = 0; i < elementos_por_processo; i++)
            soma_parcial += sub_vetor[i];

        free(sub_vetor);
    }

    printf("Processo %d: soma parcial = %d\n", rank, soma_parcial);

    /* d. O processo Um recebe os somatorios parciais, calcula e mostra o
          somatorio final dos elementos do vetor */
    if (rank == 1) {
        soma_total = soma_parcial; /* soma parcial do proprio processo 1 */

        int soma_recebida;
        /* Recebe a soma parcial do processo 0 */
        MPI_Recv(&soma_recebida, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        soma_total += soma_recebida;

        /* Recebe as somas parciais dos demais processos (2, 3, ..., size-1) */
        for (int src = 2; src < size; src++) {
            MPI_Recv(&soma_recebida, 1, MPI_INT, src, 1, MPI_COMM_WORLD, &status);
            soma_total += soma_recebida;
        }

        printf("Somatorio final dos elementos do vetor = %d\n", soma_total);
    } else {
        /* Todos os demais processos (inclusive o 0) enviam sua soma parcial
           ao processo Um */
        MPI_Send(&soma_parcial, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
