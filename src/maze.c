#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include "maze.h"

/*
    Set a unsigned int as a seed for srand() function.
    @param seed: Pointer to the unsigned int.
*/
void set_seed(unsigned int *seed)
{
    FILE *urandom = fopen("/dev/urandom", "r");
    fread(seed, sizeof(int), 1, urandom);
    fclose(urandom);
}

/*
    Return a pointer to a Cell struct given integers x and y.
    @param x: First integer (Rows).
    @param y: Second integer (Column).
    @return: The pointer to the cell.
*/
Cell* create_cell(int x, int y)
{
    Cell *cell = malloc(sizeof(Cell));
    cell->x = x;
    cell->y = y;
    cell->type = WALL;
    cell->visited_dfs = 0;
    cell->visited_bfs = 0;
    return cell;
}

/*
    Reset the visit flag of a cell.
    @param cell: Pointer to the cell.
*/
void reset_cell(Cell *cell)
{
    cell->visited_bfs = 0;
    cell->visited_dfs = 0;
}

/*
    Allocate memory for the Maze struct.
    @param m: Pointer to the Maze struct.
    @param rows: Number of rows of the maze.
    @param cols: Number of cols of the maze.
*/
void allocate_maze(Maze *m, int rows, int cols)
{
    m->rows = rows;
    m->cols = cols;
    m->matrix = (Cell***) malloc(rows * sizeof(Cell**));

    int i, j;
    for(i=0; i<rows; i++)
    {
        m->matrix[i] = (Cell**) malloc(cols * sizeof(Cell*));
        for (j=0; j<cols; j++)
        {
            m->matrix[i][j] = create_cell(i, j);
        }
    }

}

/*
    Fill a maze with random numbers between 0 and RAND_MAX.
    @param m: Pointer to the Maze struct.
*/
void fill_matrix_random(Maze *m)
{
    unsigned int seed;
    set_seed(&seed);
    srand(seed);

    int i, j;
    for (i=0; i<m->rows; i++)
    {
        for (j=0; j<m->cols; j++)
        {
            m->matrix[i][j]->type = rand();
        }
    }
}

/*
    Print the values of the maze in the console.
    @param m: Pointer to the Maze struct.
*/
void print_maze(Maze *m)
{
    int i, j;
    for (i=0; i<m->rows; i++)
    {
        for (j=0; j<m->cols; j++)
        {
            printf("%d\t", m->matrix[i][j]->type);
        }
        printf("\n");
    }
}

/*
    Print a maze graphically as a maze.
    @param m: Pointer to the Maze struct.
*/
void print_maze_graphical(Maze *m)
{
    int i, j;
    for (i=0; i<m->rows; i++)
    {
        for (j=0; j<m->cols; j++)
        {
            if (m->matrix[i][j]->type == PATH)
            {
                printf("▓▓");
            }
            else if (m->matrix[i][j]->type == WALL)
            {
                printf("░░");
            }
            else
            {
                printf("▒▒");
            }
        }
        printf("\n");
    }
}

/*
    Compare two Cells. Used when inserting cells in the neighbors list to check if certain cell is already in the list.
    @param a: Pointer to the first cell.
    @param b: Pointer to the second cell.
    @return: 0 if a == b, else -1.
*/
gint compare_cells_by_position(gconstpointer a, gconstpointer b)
{
    Cell *cell_a = (Cell*) a;
    Cell *cell_b = (Cell*) b;

    if (cell_a->x == cell_b->x && cell_a->y == cell_b->y)
        return 0;
    else
        return -1;
}

/*
    Compare the value in the Maze of two Cells. Used to insert cells in the list sorted.
    @param a: Pointer to the first cell.
    @param b: Pointer to the second cell.
    @return: 0 if value of a in maze == value of b in maze
             1 if value of a in maze > value of b in maze
            -1 if value of a in maze < value of b in maze
*/
gint compare_cells_by_value(gconstpointer a, gconstpointer b)
{
    Cell *cell_a = (Cell*) a;
    Cell *cell_b = (Cell*) b;

    if (cell_a->type == cell_b->type)
        return 0;
    else if (cell_a->type > cell_b->type)
        return 1;
    else
        return -1;
}

/*
    Add a cell to the neighbors list if the cell isn't in the list.
    @param neighbors_list: Direction of a pointer to the list.
    @param neighbor: Pointer to the cell to be inserted.
    @param m: Pointer to the Maze struct.
*/
GList* add_neighbor_to_list_if_not_exist(GList* neighbors_list, Cell *neighbor, Maze *m)
{
    int exist = g_list_find_custom(neighbors_list, neighbor, compare_cells_by_position) == NULL;

    if (exist)
    {
        return g_list_insert_sorted(neighbors_list, neighbor, compare_cells_by_value);
    }
    else
    {
        return neighbors_list;
    }
}

/*
    Add the neighbors of a cell that aren't in the neighbors list to the list.
    @param cell: Pointer to the cell.
    @param neighbors_list: Direction of a pointer to the list.
    @param m: Pointer to the Maze struct.
*/
GList* add_neighbors_to_list(Cell *cell, GList* neighbors_list, Maze *m)
{
    // Vecino Izquierdo
    if (cell->x-1 >= 0 && m->matrix[cell->x-1][cell->y]->type > 0)
    {
        neighbors_list = add_neighbor_to_list_if_not_exist(neighbors_list, m->matrix[cell->x-1][cell->y], m);
    }
    // Vecino Derecho
    if (cell->x+1 < m->rows && m->matrix[cell->x+1][cell->y]->type > 0)
    {
        neighbors_list = add_neighbor_to_list_if_not_exist(neighbors_list, m->matrix[cell->x+1][cell->y], m);
    }
    // Vecino Arriba
    if (cell->y-1 >= 0 && m->matrix[cell->x][cell->y-1]->type > 0)
    {
        neighbors_list = add_neighbor_to_list_if_not_exist(neighbors_list, m->matrix[cell->x][cell->y-1], m);
    }
    // Vecino Abajo
    if (cell->y+1 < m->cols && m->matrix[cell->x][cell->y+1]->type > 0)
    {
        neighbors_list = add_neighbor_to_list_if_not_exist(neighbors_list, m->matrix[cell->x][cell->y+1], m);
    }

    return neighbors_list;
}

/*
    Get the number of maze cells that are adjacents to certain cell.
    @param cell: Pointer to the reference cell.
    @param m: Pointer to the Maze struct.
*/
int get_adjacent_maze_cells_count(Cell *cell, Maze *m)
{
    int count = 0;

    // Vecino Izquierdo
    if (cell->x-1 >= 0 && m->matrix[cell->x-1][cell->y]->type == PATH)
        count++;
    // Vecino Derecho
    if (cell->x+1 < m->rows && m->matrix[cell->x+1][cell->y]->type == PATH)
        count++;
    // Vecino Arriba
    if (cell->y-1 >= 0 && m->matrix[cell->x][cell->y-1]->type == PATH)
        count++;
    // Vecino Abajo
    if (cell->y+1 < m->cols && m->matrix[cell->x][cell->y+1]->type == PATH)
        count++;

    return count;
}

/*
    Mark as wall all the cells that aren't marked as wall or as path.
    @param m: Pointer to the Maze struct.
*/
void mark_remaining_cells_as_walls(Maze *m)
{
    int i, j;
    for (i=0; i<m->rows; i++)
    {
        for (j=0; j<m->cols; j++)
        {
            if (m->matrix[i][j]->type > 0)
            {
                m->matrix[i][j]->type = WALL;
            }
        }
    }
}

/*
    Pause the current execution for a certain microseconds.
    @param show_steps: 1 to make a delay.
                       Otherwise other number.
    @param msec: Duration of the delay in microseconds.
*/
void delay(int show_steps, useconds_t msec)
{
    if (show_steps == 1)
    {
        usleep(msec);
    }
}

/*
    Appy Prim Algorithm to a matrix and construct a maze.
    @param m: Pointer to the Maze struct.
    @param show_steps: 1 to pause the function when marking a cell as part of the maze.
                       Otherwise other number.
    @param msec: Duration of the pause in microseconds.

    Pseudocode:
        1) Fill the matrix with random numbers.
        2) Pick a random cell and mark it as part of the maze.
        3) Add the neighbors of the cell to the neighbors list.
        4) While there are cells in the list:
            4.1) Pick the cell with the lowest weight and remove it from the list.
            4.2) If the cell has one or less neighbors marked as part of the maze:
                4.2.1) Mark the cell as part of the maze.
                4.2.2) Add the neighbors of the cell to the neighbors list.
            4.3) Mark the cell as a wall.
*/
void make_maze(Maze *m, int show_steps, useconds_t msec)
{
    fill_matrix_random(m);
    delay(show_steps, msec * 2);

    unsigned int seed;
    set_seed(&seed);
    srand(seed);

    GList *neighbors_list = NULL;
    Cell *current_cell = malloc(sizeof(Cell));

    current_cell->x = rand()%m->rows;
    current_cell->y = rand()%m->cols;

    m->matrix[current_cell->x][current_cell->y]->type = PATH;
    reset_cell(m->matrix[current_cell->x][current_cell->y]);
    neighbors_list = add_neighbors_to_list(current_cell, neighbors_list, m);
    delay(show_steps, msec);

    while (g_list_length(neighbors_list) > 0)
    {
        current_cell = (Cell*) g_list_first(neighbors_list)->data;
        neighbors_list = g_list_remove(neighbors_list, current_cell);

        if ( get_adjacent_maze_cells_count(current_cell, m) <= 1 )
        {
            m->matrix[current_cell->x][current_cell->y]->type = PATH;
            reset_cell(m->matrix[current_cell->x][current_cell->y]);
            neighbors_list = add_neighbors_to_list(current_cell, neighbors_list, m);
            delay(show_steps, msec);
        }
        else
        {
            m->matrix[current_cell->x][current_cell->y]->type = WALL;
            reset_cell(m->matrix[current_cell->x][current_cell->y]);
        }
    }

    mark_remaining_cells_as_walls(m);
}

/*
    Get the pointer to a random cell of the maze that is marked as path.
    @param m: Pointer to the Maze struct.
    @return: Pointer to the cell.
*/
Cell* get_random_path_cell(Maze *m)
{
    unsigned int seed;
    set_seed(&seed);
    srand(seed);

    Cell *cell = m->matrix[rand()%m->rows][rand()%m->cols];
    while (cell->type != PATH)
    {
        cell = m->matrix[rand()%m->rows][rand()%m->cols];
    }
    return cell;
}

/*
    Put the objects (Chesee, Poison, Goal) in the maze.
    @param m: Pointer to the Maze struct.
    @param cheese_amount: Number of cheese to put in the maze.
    @param poison_amount: Number of poison to put in the maze.
*/
void put_objets_on_maze(Maze *m, int cheese_amount, int poison_amount)
{
    Cell *cell;

    int i;
    for (i=0; i<cheese_amount; i++)
    {
        cell = get_random_path_cell(m);
        cell->type = CHEESE;
    }
    for (i=0; i<poison_amount; i++)
    {
        cell = get_random_path_cell(m);
        cell->type = POISON;
    }
    cell = get_random_path_cell(m);
    cell->type = GOAL;
}

/*
    Put a mouse in a random position and set the initial speed.
    @param m: Pointer to the Maze struct.
    @param mouse: Pointer to the Mouse.
*/
void init_mouse(Maze *m, Mouse* mouse)
{
    mouse->cell = get_random_path_cell(m);
    mouse->speed = MOUSE_INIT_SPEED;
    mouse->state = MOUSE_ALIVE;
}

/*
    Add the nighbors of a given cell that are not visited by dfs algorithm to a list.
    NOTE: The cell is pushed before the neighbor, this ensures that the cell will be revisited in the backtracking.
    @param cell: The cell used to find the neighbors.
    @param neighbors_stack: The stack where the neighbors will be pushed.
    @Maze m: Pointer to the Maze.
*/
GList* push_unvisited_dfs_neighbors_to_stack(Cell *cell, GList* neighbors_stack, Maze *m)
{
    Cell* current_neighbor;

    // Vecino Izquierdo
    if (cell->x-1 >= 0 && m->matrix[cell->x-1][cell->y]->type != WALL && m->matrix[cell->x-1][cell->y]->visited_dfs == 0)
    {
        current_neighbor = m->matrix[cell->x-1][cell->y];
        neighbors_stack = g_list_append(neighbors_stack, cell);
        neighbors_stack = g_list_append(neighbors_stack, current_neighbor);
    }
    // Vecino Derecho
    if (cell->x+1 < m->rows && m->matrix[cell->x+1][cell->y]->type != WALL && m->matrix[cell->x+1][cell->y]->visited_dfs == 0)
    {
        current_neighbor = m->matrix[cell->x+1][cell->y];
        neighbors_stack = g_list_append(neighbors_stack, cell);
        neighbors_stack = g_list_append(neighbors_stack, current_neighbor);
    }
    // Vecino Arriba
    if (cell->y-1 >= 0 && m->matrix[cell->x][cell->y-1]->type != WALL && m->matrix[cell->x][cell->y-1]->visited_dfs == 0)
    {
        current_neighbor = m->matrix[cell->x][cell->y-1];
        neighbors_stack = g_list_append(neighbors_stack, cell);
        neighbors_stack = g_list_append(neighbors_stack, current_neighbor);
    }
    // Vecino Abajo
    if (cell->y+1 < m->cols && m->matrix[cell->x][cell->y+1]->type != WALL && m->matrix[cell->x][cell->y+1]->visited_dfs == 0)
    {
        current_neighbor = m->matrix[cell->x][cell->y+1];
        neighbors_stack = g_list_append(neighbors_stack, cell);
        neighbors_stack = g_list_append(neighbors_stack, current_neighbor);
    }

    return neighbors_stack;
}

/*
    Perform a Depth First Search over a maze. The initial point will be the mouse cell and the end point will be the cell in the maze marked as GOAL.
    @param m: Pointer to the Maze.
    @param mouse: Pointer to the Mouse.

    Pseudocode:
        1) Push the initial cell in the stack.
        2) While there are cells in the stack:
            2.1) Pop a cell.
            2.2) Do something with the cell.
            2.3) If the cell is not marked as visited:
                2.3.1) Mark it as visited.
                2.3.2) For all unvisited neighbors of the cell:
                    2.3.2.1) Push the cell in the stack.
                    2.3.2.2) Push the neighbor in the stack.
*/
void depth_first_search(Maze *m, Mouse* mouse)
{
    GList *stack = NULL;
    GList *top = NULL;
    stack = g_list_append(stack, mouse->cell);

    while (g_list_length(stack) > 0)
    {
        top = g_list_last(stack);
        stack = g_list_remove_link(stack, top);

        mouse->cell = (Cell*) top->data;
        if (mouse->cell->type == GOAL)
        {
            return;
        }
        else if (mouse->cell->type == CHEESE)
        {
            mouse->cell->type = PATH;
            if (mouse->speed > MOUSE_MAX_SPEED)
            {
                mouse->speed -= MOUSE_SPEED_INCREMENT;
            }
        }
        else if (mouse->cell->type == POISON)
        {
            mouse->cell->type = PATH;
            mouse->state = MOUSE_DEAD;
            return;
        }
        usleep(mouse->speed);

        if ( mouse->cell->visited_dfs == 0 )
        {
            mouse->cell->visited_dfs = 1;
            stack = push_unvisited_dfs_neighbors_to_stack(mouse->cell, stack, m);
        }
    }
}

void breadth_first_search(Maze *m, Mouse* mouse)
{
}

void random_search(Maze *m, Mouse* mouse)
{
}
