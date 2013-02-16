#include <stdio.h>

FILE __stdin = {0, {0, 0}, NULL, 0, 0};
FILE __stdout = {1, {0, 0}, NULL, 0, 0};
FILE __stderr = {2, {0, 0}, NULL, 0, 0};

FILE *stdin = &__stdin;
FILE *stdout = &__stdout;
FILE *stderr = &__stderr;

