#ifndef SUFFIXARRAY_H
#define SUFFIXARRAY_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include "timer.h"
#include "searchalg.h"

#define CHUNK_SIZE 1024

class SuffixArray : public SearchAlg {
    public:
        SuffixArray();
        bool loadFile(char *filename);
        unsigned char **search(unsigned char *substr);
    private:
        unsigned int *array, size;

        void swap(unsigned int i, unsigned int j);
        char *getFilename(unsigned int index);
        unsigned int writeChunk(unsigned int *indexFile);
        unsigned int *readChunk(unsigned int index);
        void removeChunk(unsigned int index);
        bool mergeChunks(unsigned *iA, unsigned int *iB);
};

#endif
