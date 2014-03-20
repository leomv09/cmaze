#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "args.h"

int is_digit(char* string)
{
    int i;
    int len = strlen(string);
    for(i=0; i<len; i++)
    {
        if ( !(isdigit(string[i])) )
            return 0;
    }
    return 1;
}

int check_args(int argc, char* argv[])
{
    if (argc != 5)
    {
        printf("ERROR: Cantidad de argumentos inválida.\n");
        return 0;
    }

    int i, current;

    for (i=1; i<argc; i++)
    {
        if (!is_digit(argv[i]))
        {
            printf("ERROR: Argumento #%d inválido. No es un número positivo (%s).\n", i, argv[i]);
            return 0;
        }
        current = atoi(argv[i]);
        if (i != 4 && (current < 0 || current > 50))
        {
            printf("ERROR: Argumento #%d inválido. Este argumento debe ser un número entre 0 y 50.\n", i);
            return 0;
        }
        if (i == 4 && (current < 0 || current > 3))
        {
            printf("ERROR: Argumento #%d inválido. Este argumento debe ser un número entre 0 y 3.\n", i);
            return 0;
        }
    }

    return 1;
}

void parse_args(int argc, char* argv[], int arg_array[])
{
    int i;
    for (i=1; i<argc; i++)
    {
        arg_array[i-1] = atoi(argv[i]);
    }
}
