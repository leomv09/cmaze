#ifndef MAZE_H_INCLUDED
#define MAZE_H_INCLUDED

#include <unistd.h>

typedef struct
{
    int rows;
    int cols;
    int** matrix;
} Matrix;

typedef struct
{
    int x;
    int y;
} Mouse;

void init_matrix(Matrix *m, int rows, int cols);
void print_matrix(Matrix *m);
void print_matrix_as_maze(Matrix *m);
void make_maze(Matrix *m, int show_steps, useconds_t delay);
void put_objets_on_maze(Matrix *m, int cheese_amount, int poison_amount);
void init_mouse(Matrix *m, Mouse* mouse);

void depth_first_search(Matrix *m, Mouse* mouse);
void breadth_first_search(Matrix *m, Mouse* mouse);
void random_search(Matrix *m, Mouse* mouse);

#endif // MAZE_H_INCLUDED
