#ifndef BMH_H
#define BMH_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "timer.h"

#define BHM_LIMIT 1000
#define MAX_STRING_MAX_LENGTH 4096

class StringNode {
    public:
        char *s;
        StringNode *next;
};

class BMH {
    public:
        BMH();
        bool load_file(char *filename);
        char **search(char *substr);


    private:
        char *full_text;
        unsigned int size;
        StringNode *stringList[MAX_STRING_MAX_LENGTH], *snode, *snodeAux,
                   *stringListIndex[BHM_LIMIT];
};


#endif
