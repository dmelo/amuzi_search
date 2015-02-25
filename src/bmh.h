#ifndef BMH_H
#define BMH_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include "timer.h"
#include "searchalg.h"

#define MAX_STRING_MAX_LENGTH 4096

class BMH : public SearchAlg {
    public:
        BMH();
        bool loadFile(char *filename);
        unsigned char **search(unsigned char *substr);

    private:
        unsigned char *full_text, bad_char_skip[UCHAR_MAX + 1];
        unsigned int size;
        StringNode *stringList[MAX_STRING_MAX_LENGTH], *snode, *snodeAux,
                   *stringListIndex[SEARCH_LIMIT];
};


#endif
