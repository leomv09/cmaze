#ifndef MAZE_H_INCLUDED
#define MAZE_H_INCLUDED

#include <unistd.h>

#define MOUSE_INIT_SPEED 120000
#define MOUSE_MAX_SPEED 80000
#define MOUSE_SPEED_INCREMENT 5000

typedef enum {WALL = -1, PATH, GOAL, CHEESE, POISON} CellType;
typedef enum {MOUSE_ALIVE, MOUSE_DEAD} MouseState;

typedef struct
{
    int x;
    int y;
    int visited_dfs;
    int visited_bfs;
    CellType type;
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
    MouseState state;
    useconds_t speed;
} Mouse;

void allocate_maze(Maze *m, int rows, int cols);
void print_maze(Maze *m);
void print_maze_graphical(Maze *m);
void make_maze(Maze *m, int cheese_amount, int poison_amount, int show_steps);
void init_mice(Maze *m, Mouse **mouse);
int depth_first_search(Maze *m, Mouse *mouse);
int breadth_first_search(Maze *m, Mouse *mouse);
int random_search(Maze *m, Mouse *mouse);

#endif // MAZE_H_INCLUDED
