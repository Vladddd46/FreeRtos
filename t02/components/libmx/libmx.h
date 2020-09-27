#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


char *mx_string_copy(char *str);
char *mx_strnew(int size);
char **mx_strsplit( char *s, char c);
char **mx_strarr_new(int size);