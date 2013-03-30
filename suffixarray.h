#ifndef SUFFIXARRAY_H
#define SUFFIXARRAY_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include "timer.h"
#include "searchalg.h"

#define SORT_FUNCTION mergeSort
#define PRESORT_CHUNKS 1

class SuffixArray : public SearchAlg {
    public:
        SuffixArray();
        bool loadFile(char *filename);
        uchar **search(uchar *substr);
    private:
        uint *array, chunkTmp[CHUNK_SIZE * 2], chunkA[CHUNK_SIZE],
             chunkB[CHUNK_SIZE];

        void swap(uint i, uint j);
        char *getFilename(uint index, char *filename);
        uint writeChunk(uint *indexFile);
        void readChunk(uint index, uint varIndex);
        uint sortChunk(uint index);
        void removeChunk(uint index);
        bool mergeChunks(uint *iA, uint *iB);

        bool loadState(uint *i, uint *count);
        bool saveState(uint i, uint count, uint total);

        bool matchFound(uint index, uchar *substr);
        uint binarySearch(uint *list, uint l, uint r, uchar *substr);

        void pick(uint index);
};

#endif
