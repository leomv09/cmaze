#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "args.h"

/*
    Check if a string is a number. Foreach digit check if is a valid digit. No negative number allowed.
    @param string: String to check.
*/
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

/*
    Check some restrictions on the command line argument.
    @param argc: Number of arguments.
    @param argv: Array of arguments.
*/
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

/*
    Parse the command line arguments as integers and store it in a Argument struct.
    @param argc: Number of arguments.
    @param argv: Array of arguments.
    @param args: Struct to store the parsed arguments.
*/
void parse_args(int argc, char* argv[], Arguments* args)
{
    args->rows = atoi(argv[1]);
    args->cols = atoi(argv[2]);
    args->cheese = atoi(argv[3]);
    args->poison = atoi(argv[4]);
}
