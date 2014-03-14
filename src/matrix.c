#include <stdlib.h>
#include <stdio.h>
#include "matrix.h"

void set_seed(unsigned int* seed)
{
    FILE* urandom = fopen("/dev/urandom", "r");
    fread(seed, sizeof(int), 1, urandom);
    fclose(urandom);
}

void fill_matrix_random(int rows, int cols, int mtx[rows][cols])
{
    unsigned int seed;
    set_seed(&seed);
    srand(seed);
    int i, j;
    for (i=0; i<rows; i++)
    {
        for (j=0; j<cols; j++)
        {
            mtx[i][j] = rand();
        }
    }
}

void print_matrix(int rows, int cols, int mtx[rows][cols])
{
    int i, j;
    for (i=0; i<rows; i++)
    {
        for (j=0; j<cols; j++)
        {
            printf("%d\t", mtx[i][j]);
        }
        printf("\n");
    }
}
