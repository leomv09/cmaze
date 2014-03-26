#ifndef ARGS_H_INCLUDED
#define ARGS_H_INCLUDED

typedef struct
{
    int rows;
    int cols;
    int cheese;
    int poison;
} Arguments;

int check_args(int argc, char* argv[]);
void parse_args(int argc, char* argv[], Arguments* arg_struct);

#endif // ARGS_H_INCLUDED
