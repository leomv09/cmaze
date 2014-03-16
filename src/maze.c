#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include "maze.h"

typedef struct
{
    int x;
    int y;
} Cell;

/*
    Set a unsigned int as a seed for srand() function.
    @param seed: Pointer to the unsigned int.
*/
void set_seed(unsigned int* seed)
{
    FILE* urandom = fopen("/dev/urandom", "r");
    fread(seed, sizeof(int), 1, urandom);
    fclose(urandom);
}

/*
    Allocate memory for the Matrix struct.
    @param m: Pointer to the Matrix struct.
    @param rows: Number of rows of the matrix.
    @param cols: Number of cols of the matrix.
*/
void init_matrix(Matrix* m, int rows, int cols)
{
    m->rows = rows;
    m->cols = cols;
    m->matrix = (int**) malloc(rows * sizeof(int*));

    int i;
    for(i=0; i<rows; i++)
    {
        m->matrix[i] = (int*) malloc(cols * sizeof(int));
    }
}

/*
    Fill a matrix with random numbers between 1 and RAND_MAX.
    @param m: Pointer to the Matrix struct.
*/
void fill_matrix_random(Matrix* m)
{
    unsigned int seed;
    set_seed(&seed);
    srand(seed);

    int i, j;
    for (i=0; i<m->rows; i++)
    {
        for (j=0; j<m->cols; j++)
        {
            m->matrix[i][j] = rand() + 1;
        }
    }
}

/*
    Print the values of the matrix in the console.
    @param m: Pointer to the Matrix struct.
*/
void print_matrix(Matrix* m)
{
    int i, j;
    for (i=0; i<m->rows; i++)
    {
        for (j=0; j<m->cols; j++)
        {
            printf("%d\t", m->matrix[i][j]);
        }
        printf("\n");
    }
}

/*
    Print a matrix graphically as a maze. (0 == path, other number == wall)
    @param m: Pointer to the Matrix struct.
*/
void print_matrix_as_maze(Matrix* m)
{
    int i, j;
    for (i=0; i<m->rows; i++)
    {
        for (j=0; j<m->cols; j++)
        {
            if (m->matrix[i][j] == 0)
            {
                printf("░░");
            }
            else
            {
                printf("▓▓");
            }
        }
        printf("\n");
    }
}

/*
    Return a pointer to a Cell struct given integers x and y.
    @param x: First integer.
    @param y: Second integer.
    @return: The pointer to the cell.
*/
Cell* get_Cell(int x, int y)
{
    Cell* cell = malloc(sizeof(Cell));
    cell->x = x;
    cell->y = y;
    return cell;
}

/*
    Compare two Cells. Used when inserting cells in the neighbors list to check if certain cell is already in the list.
    @param a: Pointer to the first cell.
    @param b: Pointer to the second cell.
    @return: 0 if a == b, else -1.
*/
gint compare_cells(gconstpointer a, gconstpointer b)
{
    Cell* cell_a = (Cell*) a;
    Cell* cell_b = (Cell*) b;

    if (cell_a->x == cell_b->x && cell_a->y == cell_b->y)
        return 0;
    else
        return -1;
}

/*
    Compare the value in the Matrix of two Cells. Used to insert cells in the list sorted.
    @param a: Pointer to the first cell.
    @param b: Pointer to the second cell.
    @param data: Pointer to the Matrix struct.
    @return: 0 if value of a in matrix == value of b in matrix
             1 if value of a in matrix > value of b in matrix
            -1 if value of a in matrix < value of b in matrix
*/
gint compare_value_of_cells_in_matrix(gconstpointer a, gconstpointer b, gpointer data)
{
    Cell* cell_a = (Cell*) a;
    Cell* cell_b = (Cell*) b;
    Matrix* m = (Matrix*) data;

    if (m->matrix[cell_a->x][cell_a->y] == m->matrix[cell_b->x][cell_b->y])
        return 0;
    else if (m->matrix[cell_a->x][cell_a->y] > m->matrix[cell_b->x][cell_b->y])
        return 1;
    else
        return -1;
}

/*
    Mark a cell as path in the maze.
    @param cell: Pointer to the cell to be marked.
    @param m: Pointer to the Matrix struct.
*/
void mark_cell_as_maze(Cell* cell, Matrix* m)
{
    m->matrix[cell->x][cell->y] = 0;
    printf("Marked as maze.\n");
}

/*
    Mark a cell as wall in the maze.
    @param cell: Cell to be marked.
    @param m: Pointer to the Matrix struct.
*/
void mark_cell_as_wall(Cell* cell, Matrix* m)
{
    m->matrix[cell->x][cell->y] = -1;
    printf("Marked as wall.\n");
}

/*
    Add a cell to the neighbors list if the cell isn't in the list.
    @param neighbors_list: Direction of a pointer to the list.
    @param neighbor: Pointer to the cell to be inserted.
    @param m: Pointer to the Matrix struct.
*/
void add_neighbor_to_list_if_not_exist(GList** neighbors_list, Cell* neighbor, Matrix* m)
{
    if (g_list_find_custom(*neighbors_list, neighbor, compare_cells) == NULL)
    {
        *neighbors_list = g_list_insert_sorted_with_data(*neighbors_list, neighbor, compare_value_of_cells_in_matrix, m);
        printf("   Cell (%d, %d) added to neighbors list (Length %d).\n", neighbor->x, neighbor->y, g_list_length(*neighbors_list));
    }
    else
    {
        printf("   Cell (%d, %d) already is in neighbors list (Length %d).\n", neighbor->x, neighbor->y, g_list_length(*neighbors_list));
    }
}

/*
    Add the neighbors of a cell that aren't in the neighbors list to the list.
    @param cell: Pointer to the cell.
    @param neighbors_list: Direction of a pointer to the list.
    @param m: Pointer to the Matrix struct.
*/
void add_neighbors_to_list(Cell* cell, GList** neighbors_list, Matrix* m)
{
    printf("Getting neighbors...\n");
    Cell* current_neighbor;

    if (cell->x-1 >= 0 && m->matrix[cell->x-1][cell->y] > 0)
    {
        printf("   Cell (%d, %d) is neighbor.\n", cell->x-1, cell->y);
        current_neighbor = get_Cell(cell->x-1, cell->y);
        add_neighbor_to_list_if_not_exist(neighbors_list, current_neighbor, m);
    }
    if (cell->x+1 < m->rows && m->matrix[cell->x+1][cell->y] > 0)
    {
        printf("   Cell (%d, %d) is neighbor.\n", cell->x+1, cell->y);
        current_neighbor = get_Cell(cell->x+1, cell->y);
        add_neighbor_to_list_if_not_exist(neighbors_list, current_neighbor, m);
    }
    if (cell->y-1 >= 0 && m->matrix[cell->x][cell->y-1] > 0)
    {
        printf("   Cell (%d, %d) is neighbor.\n", cell->x, cell->y-1);
        current_neighbor = get_Cell(cell->x, cell->y-1);
        add_neighbor_to_list_if_not_exist(neighbors_list, current_neighbor, m);
    }
    if (cell->y+1 < m->cols && m->matrix[cell->x][cell->y+1] > 0)
    {
        printf("   Cell (%d, %d) is neighbor.\n", cell->x, cell->y+1);
        current_neighbor = get_Cell(cell->x, cell->y+1);
        add_neighbor_to_list_if_not_exist(neighbors_list, current_neighbor, m);
    }
}

/*
    Get the number of maze cells that are adjacents to certain cell.
    @param cell: Pointer to the reference cell.
    @param m: Pointer to the Matrix struct.
*/
int get_adjacent_maze_cells_count(Cell* cell, Matrix* m)
{
    int count = 0;

    if (cell->x-1 >= 0 && m->matrix[cell->x-1][cell->y] == 0)
        count++;
    if (cell->x+1 < m->rows && m->matrix[cell->x+1][cell->y] == 0)
        count++;
    if (cell->y-1 >= 0 && m->matrix[cell->x][cell->y-1] == 0)
        count++;
    if (cell->y+1 < m->cols && m->matrix[cell->x][cell->y+1] == 0)
        count++;

    printf("Cell has %d adjacents maze cells.\n", count);
    return count;
}

/*
    Mark as wall all the cells that aren't marked as wall or as path.
    @param m: Pointer to the Matrix struct.
*/
void mark_remaining_cells_as_walls(Matrix* m)
{
    int i, j;
    for (i=0; i<m->rows; i++)
    {
        for (j=0; j<m->cols; j++)
        {
            if (m->matrix[i][j] > 0)
            {
                m->matrix[i][j] = -1;
            }
        }
    }
}

/*
    Appy Prim Algorithm to a matrix and construct a maze.
    @param m: Pointer to the Matrix struct.

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
void make_maze(Matrix* m)
{
    fill_matrix_random(m);

    unsigned int seed;
    set_seed(&seed);
    srand(seed);

    GList* neighbors_list = NULL;
    Cell* current_cell = malloc(sizeof(Cell));

    current_cell->x = rand()%m->rows;
    current_cell->y = rand()%m->cols;

    printf("Initial Cell: (%d, %d).\n", current_cell->x, current_cell->y);
    mark_cell_as_maze(current_cell, m);
    add_neighbors_to_list(current_cell, &neighbors_list, m);
    printf("\n");

    while (g_list_length(neighbors_list) > 0)
    {
        current_cell = (Cell*) g_list_first(neighbors_list)->data;
        printf("Current Cell: (%d, %d).\n", current_cell->x, current_cell->y);
        neighbors_list = g_list_remove(neighbors_list, current_cell);
        printf("Cell removed from neighbors list (Length %d).\n", g_list_length(neighbors_list));

        if ( get_adjacent_maze_cells_count(current_cell, m) <= 1 )
        {
            mark_cell_as_maze(current_cell, m);
            add_neighbors_to_list(current_cell, &neighbors_list, m);
        }
        else
        {
            mark_cell_as_wall(current_cell, m);
        }
        printf("\n");
    }

    mark_remaining_cells_as_walls(m);
}
