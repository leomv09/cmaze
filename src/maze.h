#ifndef MAZE_H_INCLUDED
#define MAZE_H_INCLUDED

#include <unistd.h>

#define WALL -1
#define PATH 0
#define GOAL 1
#define CHEESE 2
#define POISON 3

#define MOUSE_INIT_SPEED 120000
#define MOUSE_MAX_SPEED 80000
#define MOUSE_SPEED_INCREMENT 5000
#define MOUSE_ALIVE 0
#define MOUSE_DEAD 1

typedef struct
{
    int x;
    int y;
    int type;
    int visited_dfs;
    int visited_bfs;
} Cell;

typedef struct
{
    int rows;
    int cols;
    Cell ***matrix;
} Maze;

typedef struct
{
    Cell *cell;
    int state;
    useconds_t speed;
} Mouse;

void allocate_maze(Maze *m, int rows, int cols);
void print_maze(Maze *m);
void print_maze_graphical(Maze *m);
void make_maze(Maze *m, int show_steps, useconds_t delay);
void put_objets_on_maze(Maze *m, int cheese_amount, int poison_amount);
void init_mouse(Maze *m, Mouse* mouse);

void depth_first_search(Maze *m, Mouse* mouse);
void breadth_first_search(Maze *m, Mouse* mouse);
void random_search(Maze *m, Mouse* mouse);

#endif // MAZE_H_INCLUDED
