#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include "maze.h"

void on_window_destroy (GtkObject *object, gpointer user_data)
{
    gtk_main_quit ();
}

int main (int argc, char* argv[])
{
    /*
    Matrix* m = malloc(sizeof(Matrix));
    init_matrix(m, 15, 25);
    print_matrix(m);
    printf("\n");
    make_maze(m);
    print_matrix_as_maze(m);
    */

    GtkBuilder* builder;
    GtkWidget* window;

    gtk_init (&argc, &argv);

    builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, "src/gui.glade", NULL);
    window = GTK_WIDGET (gtk_builder_get_object (builder, "main_window"));
    gtk_builder_connect_signals (builder, NULL);

    g_object_unref (G_OBJECT (builder));

    gtk_widget_show (window);
    gtk_main ();

    return 0;
}
