#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include "maze.h"

int main (int argc, char *argv[])
{
    Matrix* m = malloc(sizeof(Matrix));
    init_matrix(m, 15, 25);
    print_matrix(m);
    printf("\n");
    make_maze(m);
    print_matrix_as_maze(m);
    return 0;
}
