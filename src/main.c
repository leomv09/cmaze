#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include "maze.h"

#define SHOW_STEPS 1
#define DELAY 150000

int main (int argc, char *argv[])
{
    /*
    Matrix *m = malloc(sizeof(Matrix));
    init_matrix(m, 15, 25);
    print_matrix(m);
    printf("\n");
    make_maze(m, SHOW_STEPS, DELAY);
    print_matrix_as_maze(m);
    */

    GtkBuilder *builder;
    GObject *window;

    gtk_init (&argc, &argv);

    builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, "src/gui.glade", NULL);

    window = gtk_builder_get_object (builder, "main_window");
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

    gtk_main ();

    return 0;
}
