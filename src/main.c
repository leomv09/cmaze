#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include "matrix.h"

int main (int argc, char *argv[])
{
    int m_rows, m_cols;
    m_rows = 3;
    m_cols = 3;
    int mtx[m_rows][m_cols];
    fill_matrix_random(m_rows, m_cols, mtx);
    print_matrix(m_rows, m_cols, mtx);
    return 0;
}
