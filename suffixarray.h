#ifndef SUFFIXARRAY_H
#define SUFFIXARRAY_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include<pthread.h>
#include<stdint.h>
#include "timer.h"
#include "searchalg.h"

#define SORT_FUNCTION mergeSort
#define PRESORT_CHUNKS 1
#define N_THREAD 2

class SuffixArray : public SearchAlg {
    public:
        uint *array, count, total;
        pthread_mutex_t lock;

        SuffixArray();
        bool loadFile(char *filename);
        uchar **search(uchar *substr);
    private:
        uint chunkTmp[N_THREAD][CHUNK_SIZE * 2], chunkA[N_THREAD][CHUNK_SIZE],
             chunkB[N_THREAD][CHUNK_SIZE];

        void swap(uint i, uint j);
        char *getFilename(uint index, char *filename);
        uint writeChunk(uint *indexFile);
        void readChunk(uint index, uint varIndex, uint tid);
        uint sortChunk(uint index);
        void removeChunk(uint index);
        bool mergeChunks(uint *iA, uint *iB, uint tid);
        static void workerMerge(void *ptr);

        bool loadState(uint *i, uint *count);
        bool saveState(uint i, uint count, uint total);

        bool matchFound(uint index, uchar *substr);
        uint binarySearch(uint *list, uint l, uint r, uchar *substr);

        void pick(uint index);
};

struct ThreadType {
    SuffixArray *obj;
    uint i;
    uint tid;
};

#endif
