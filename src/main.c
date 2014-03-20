#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include "args.h"
#include "maze.h"

#define DELAY 100000
#define ANIMATED 1
#define NOT_ANIMATED 0
#define ROWS 0
#define COLS 1
#define CHEESE 2
#define POISON 3

static void draw_maze(GtkWidget *widget, cairo_t *cr, Matrix *m)
{
    int height = gtk_widget_get_allocated_height(widget);
    int width = gtk_widget_get_allocated_width(widget);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_rectangle(cr, 0, 0, width, height);
    cairo_fill(cr);
    cairo_set_source_rgb(cr, 1, 1, 1);

    int cell_height =  height/m->rows;
    int cell_width = width/m->cols;
    int x = 0;
    int y = 0;

    int i, j;
    for(i=0; i<m->rows; i++)
    {
        for(j=0; j<m->cols; j++)
        {
            if(m->matrix[i][j] == 0)
            {
                cairo_rectangle(cr, x, y, cell_width, cell_height);
                cairo_fill(cr);
                cairo_stroke(cr);
            }
            x += cell_width;
        }
        x = 0;
        y += cell_height;
    }
}

static gboolean draw_maze_timer(GtkWidget *widget)
{
  gtk_widget_queue_draw(widget);
  return TRUE;
}

static void create_maze(GtkWidget *widget, Matrix *m)
{
    make_maze(m, NOT_ANIMATED, DELAY);
}

static void create_maze_animated(GtkWidget *widget, Matrix *m)
{
    make_maze(m, ANIMATED, DELAY);
}

int main(int argc, char *argv[])
{
    int check = check_args(argc, argv);

    if (check)
    {
        int args[4];
        parse_args(argc, argv, args);

        Matrix *m = malloc(sizeof(Matrix));
        init_matrix(m, args[ROWS], args[COLS]);

        gtk_init (&argc, &argv);

        GtkBuilder *builder = gtk_builder_new ();
        gtk_builder_add_from_file (builder, "src/gui.glade", NULL);

        GObject *window, *drawing_area, *button_create, *button_create_animated;

        window = gtk_builder_get_object (builder, "main_window");
        g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

        drawing_area = gtk_builder_get_object (builder, "drawing_area");
        g_signal_connect(drawing_area, "draw", G_CALLBACK(draw_maze), m);

        button_create = gtk_builder_get_object (builder, "button_create");
        g_signal_connect(button_create, "clicked", G_CALLBACK(create_maze), m);

        button_create_animated = gtk_builder_get_object (builder, "button_create_animated");
        g_signal_connect(button_create_animated, "clicked", G_CALLBACK(create_maze_animated), m);

        g_timeout_add(33, (GSourceFunc) draw_maze_timer, (gpointer) GTK_WIDGET(drawing_area));
        gtk_widget_show_all(GTK_WIDGET(window));

        gtk_main();

        return 0;
    }
    else
    {
        return -1;
    }
}
