#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include "args.h"
#include "maze.h"

#define DELAY 9000
#define ANIMATED 1
#define NOT_ANIMATED 0

typedef struct
{
    Matrix* m;
    Arguments* args;
    Mouse** mice;
} Global_Data;

static int animation_running = 0;

static void draw_maze(GtkWidget *widget, cairo_t *cr, Global_Data *data)
{
    int height = gtk_widget_get_allocated_height(widget);
    int width = gtk_widget_get_allocated_width(widget);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_rectangle(cr, 0, 0, width, height);
    cairo_fill(cr);
    cairo_set_source_rgb(cr, 1, 1, 1);

    int cell_height =  height/data->m->rows;
    int cell_width = width/data->m->cols;
    int x = 0;
    int y = 0;

    int i, j;
    for(i=0; i<data->m->rows; i++)
    {
        for(j=0; j<data->m->cols; j++)
        {
            if(data->m->matrix[i][j] == 0)
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

static gboolean redraw_widget(GtkWidget *widget)
{
    gtk_widget_queue_draw(widget);
    return TRUE;
}

void* create_maze_animated_thread(void *param)
{
    animation_running = 1;

    int i;
    Global_Data *data = (Global_Data*) param;
    make_maze(data->m, ANIMATED, DELAY);
    put_objets_on_maze(data->m, 5, 1);
    for (i=0; i<3; i++)
    {
        init_mouse(data->m, data->mice[i]);
    }

    animation_running = 0;
    pthread_exit(0);
}

static void create_maze_animated(GtkWidget *widget, Global_Data *data)
{
    if (animation_running == 0)
    {
        pthread_t animated_thread;
        pthread_create(&animated_thread, NULL, create_maze_animated_thread, data);
    }
}

static void create_maze(GtkWidget *widget, Global_Data *data)
{
    if (animation_running == 0)
    {
        int i;
        make_maze(data->m, NOT_ANIMATED, DELAY);
        put_objets_on_maze(data->m, 5, 1);
        for (i=0; i<3; i++)
        {
            init_mouse(data->m, data->mice[i]);
        }
    }
}

void* start_depth_thread(void *param)
{
    Global_Data *data = (Global_Data*) param;
    depth_first_search(data->m, data->mice[0]);
    pthread_exit(0);
}

void* start_breadth_thread(void *param)
{
    Global_Data *data = (Global_Data*) param;
    breadth_first_search(data->m, data->mice[1]);
    pthread_exit(0);
}

void* start_random_thread(void *param)
{
    Global_Data *data = (Global_Data*) param;
    random_search(data->m, data->mice[2]);
    pthread_exit(0);
}

static void start_simulation(GtkWidget *widget, Global_Data *data)
{
    if (animation_running == 0)
    {
        animation_running = 1;

        pthread_t* threads = malloc(3 * sizeof(pthread_t));
        pthread_create(&threads[0], NULL, start_depth_thread, data);
        pthread_create(&threads[1], NULL, start_breadth_thread, data);
        pthread_create(&threads[2], NULL, start_random_thread, data);

        animation_running = 0;
    }
}

int main(int argc, char *argv[])
{
    int check = check_args(argc, argv);

    if (check)
    {
        Arguments* args = malloc(sizeof(Arguments));
        Matrix *m = malloc(sizeof(Matrix));
        Mouse **mice = malloc(3 * sizeof(Mouse*));

        int i;
        for (i=0; i<3; i++)
        {
            mice[i] = malloc(sizeof(Mouse));
        }

        Global_Data* data = malloc(sizeof(Global_Data));
        data->args = args;
        data->m = m;
        data->mice = mice;

        parse_args(argc, argv, args);
        init_matrix(m, args->rows, args->cols);
        gtk_init (&argc, &argv);

        GtkBuilder *builder = gtk_builder_new ();
        gtk_builder_add_from_file (builder, "src/gui.glade", NULL);

        GObject *window, *drawing_area, *button_create, *button_create_animated, *button_start;

        window = gtk_builder_get_object (builder, "main_window");
        g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

        drawing_area = gtk_builder_get_object (builder, "drawing_area");
        g_signal_connect(drawing_area, "draw", G_CALLBACK(draw_maze), data);

        button_create = gtk_builder_get_object (builder, "button_create");
        g_signal_connect(button_create, "clicked", G_CALLBACK(create_maze), data);

        button_create_animated = gtk_builder_get_object (builder, "button_create_animated");
        g_signal_connect(button_create_animated, "clicked", G_CALLBACK(create_maze_animated), data);

        button_start = gtk_builder_get_object (builder, "button_start");
        g_signal_connect(button_start, "clicked", G_CALLBACK(start_simulation), data);

        g_timeout_add(33, (GSourceFunc) redraw_widget, (gpointer) GTK_WIDGET(drawing_area));
        gtk_widget_show_all(GTK_WIDGET(window));

        gtk_main();

        return 0;
    }
    else
    {
        return -1;
    }
}
