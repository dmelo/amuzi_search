#ifndef SUFFIXARRAY_H
#define SUFFIXARRAY_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "timer.h"
#include "searchalg.h"

#define S_PER_FILE 1024

class SuffixArray : public SearchAlg {
    public:
        SuffixArray();
        bool loadFile(char *filename);
        unsigned char **search(unsigned char *substr);
    private:
        unsigned char *full_text;
        unsigned int *array, size;

        void swap(unsigned int i, unsigned int j);
        bool recordPieceFile(unsigned int indexFile[S_PER_FILE]);
};

#endif
