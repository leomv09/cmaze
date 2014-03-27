#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include "args.h"
#include "maze.h"

#define ANIMATED 1
#define NOT_ANIMATED 0

/*
######################################################
# GLOBAL DATA                                        #
######################################################
*/

typedef struct
{
    Maze* m;
    Arguments* args;
    Mouse** mice;
} Global_Data;

static int animation_running = 0;
static int maze_created = 0;

/*
######################################################
# DRAWING                                            #
######################################################
*/

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

    int i, j;
    Cell* current_cell;
    for (i=0; i<data->m->rows; i++)
    {
        for (j=0; j<data->m->cols; j++)
        {
            current_cell = data->m->matrix[i][j];
            if (current_cell->type == PATH)
            {
                cairo_rectangle(cr, current_cell->x * cell_width, current_cell->y * cell_height, cell_width, cell_height);
                cairo_fill(cr);
                cairo_stroke(cr);
            }
        }
    }
}

static gboolean redraw_widget(GtkWidget *widget)
{
    gtk_widget_queue_draw(widget);
    return TRUE;
}

/*
######################################################
# MAZE CREATION                                      #
######################################################
*/

void* create_maze_animated_thread(void *param)
{
    animation_running = 1;
    maze_created = 0;

    Global_Data *data = (Global_Data*) param;
    make_maze(data->m, data->args->cheese, data->args->poison, ANIMATED);
    init_mice(data->m, data->mice);

    animation_running = 0;
    maze_created = 1;
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
        maze_created = 0;

        make_maze(data->m, data->args->cheese, data->args->poison, NOT_ANIMATED);
        init_mice(data->m, data->mice);

        maze_created = 1;
    }
}

/*
######################################################
# MAZE SOLVING                                       #
######################################################
*/

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
    if (maze_created == 1)
    {
        pthread_t* threads = malloc(3 * sizeof(pthread_t));
        pthread_create(&threads[0], NULL, start_depth_thread, data);
        pthread_create(&threads[1], NULL, start_breadth_thread, data);
        pthread_create(&threads[2], NULL, start_random_thread, data);
    }
}

/*
######################################################
# MAIN                                               #
######################################################
*/

int main(int argc, char *argv[])
{
    int check = check_args(argc, argv);

    if (check)
    {
        // Program arguments.
        Arguments* args = malloc(sizeof(Arguments));
        parse_args(argc, argv, args);

        // Maze.
        Maze *m = malloc(sizeof(Maze));
        allocate_maze(m, args->rows, args->cols);

        // Array of Mice.
        Mouse **mice = malloc(3 * sizeof(Mouse*));
        int i;
        for (i=0; i<3; i++)
        {
            mice[i] = malloc(sizeof(Mouse));
        }

        // Data that will be passed to the signals.
        Global_Data* data = malloc(sizeof(Global_Data));
        data->args = args;
        data->m = m;
        data->mice = mice;

        // User Interface
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
