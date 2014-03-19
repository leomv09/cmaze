#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include "maze.h"

#define DELAY 150000

static void do_drawing(GtkWidget *widget, cairo_t *cr, Matrix *m);
static void create_maze(GtkWidget *widget, Matrix *m);
static void create_maze_animated(GtkWidget *widget, Matrix *m);

static void do_drawing(GtkWidget *widget, cairo_t *cr, Matrix *m)
{
    int width, height;
    height = gtk_widget_get_allocated_height(widget);
    width = gtk_widget_get_allocated_width(widget);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_rectangle(cr, 0, 0, width, height);
    cairo_fill(cr);
    cairo_set_source_rgb(cr, 1, 1, 1);

    int posx, posy, cell_height, cell_width, i, j;
    cell_height =  height/m->rows;
    cell_width = width/m->cols;
    posx = 0;
    posy = 0;
    for(i = 0; i < m->rows; i++)
    {
        for(j = 0; j < m->cols; j++)
        {
            if(m->matrix[i][j] == 0)
            {
                cairo_rectangle(cr, posx, posy, cell_width, cell_height);
                cairo_fill(cr);
                cairo_stroke(cr);
            }
            posx += cell_width;
        }
        posx = 0;
        posy += cell_height;
    }
}

static void create_maze(GtkWidget *widget, Matrix *m)
{
    make_maze(m, 0, DELAY);
    GtkWidget *ventana = gtk_widget_get_toplevel(widget);
    gtk_widget_queue_draw(ventana);
}

static void create_maze_animated(GtkWidget *widget, Matrix *m)
{
    make_maze(m, 1, DELAY);
    GtkWidget *ventana = gtk_widget_get_toplevel(widget);
    gtk_widget_queue_draw(ventana);
}

int main(int argc, char *argv[])
{
    Matrix *m = malloc(sizeof(Matrix));
    init_matrix(m, 50, 50);

    GtkBuilder *builder;
    GObject *window, *button_create, *button_create_animated;

    gtk_init (&argc, &argv);

    builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, "src/gui.glade", NULL);

    window = gtk_builder_get_object (builder, "main_window");
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

    GObject *darea = gtk_builder_get_object (builder, "darea");
    g_signal_connect(darea, "draw", G_CALLBACK(do_drawing), m);

    button_create = gtk_builder_get_object (builder, "button_create");
    g_signal_connect(button_create, "clicked", G_CALLBACK(create_maze), m);

    button_create_animated = gtk_builder_get_object (builder, "button_create_animated");
    g_signal_connect(button_create_animated, "clicked", G_CALLBACK(create_maze_animated), m);

    gtk_main();

    return 0;
}
