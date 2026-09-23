/*
 * Lista de Exercicios II - Exercicio 2.a
 * Cada processo i envia uma mensagem para o proximo processo, formando
 * um anel logico. Versao com uso da estrutura de desvio "if" para
 * evitar deadlock (pares enviam primeiro, impares recebem primeiro).
 *
 * Compilar: mpicc lista2_2a.c -o lista2_2a
 * Executar: mpirun -np 4 ./lista2_2a
 */

#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int valor_enviado, valor_recebido;
    int proximo, anterior;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    proximo = (rank + 1) % size;
    anterior = (rank - 1 + size) % size;
    valor_enviado = rank;

    /* Uso de if para evitar deadlock: processos pares enviam primeiro,
       processos impares recebem primeiro */
    if (rank % 2 == 0) {
        MPI_Send(&valor_enviado, 1, MPI_INT, proximo, 0, MPI_COMM_WORLD);
        MPI_Recv(&valor_recebido, 1, MPI_INT, anterior, 0, MPI_COMM_WORLD, &status);
    } else {
        MPI_Recv(&valor_recebido, 1, MPI_INT, anterior, 0, MPI_COMM_WORLD, &status);
        MPI_Send(&valor_enviado, 1, MPI_INT, proximo, 0, MPI_COMM_WORLD);
    }

    printf("Processo %d enviou %d para %d e recebeu %d de %d\n",
           rank, valor_enviado, proximo, valor_recebido, anterior);

    MPI_Finalize();
    return 0;
}
