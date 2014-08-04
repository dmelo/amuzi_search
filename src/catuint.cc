#include<stdio.h>
#include<stdlib.h>
#include"defs.h"

int main(int argc, char **argv)
{
    FILE *fd;
    int size;
    uint *list;

    if ((fd = fopen(argv[1], "rb")) != NULL) {
        fseek(fd, 0, SEEK_END);
        size = ftell(fd);
        fseek(fd, 0, SEEK_SET);
        list = (uint *) malloc(sizeof(uint) * size);
        fread(list, sizeof(uint), size, fd);

        for (uint i = 0; i < size / sizeof(uint); i++) {
            printf("%u\n", list[i]);
        }
    }

    return 0;
}
