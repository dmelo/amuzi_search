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
        pthread_barrier_t barrier, barrier2;

        SuffixArray();
        bool loadFile(char *filename);
        uchar **search(uchar *substr);

        /**
         * Save current state on disk.
         */
        bool saveState(uint i, uint count, uint total);


    private:
        uint chunkTmp[N_THREAD][CHUNK_SIZE * 2], chunkA[N_THREAD][CHUNK_SIZE],
             chunkB[N_THREAD][CHUNK_SIZE];

        /**
         * Swap two positions on array.
         */
        void swap(uint i, uint j);

        /**
         * Calculate the file name.
         */
        void getFilename(uint index, char *filename);

        /**
         * Write chunk on file.
         */
        uint writeChunk(uint *indexFile);

        /**
         * Read chunk from file.
         */
        void readChunk(uint index, uint varIndex, uint tid);

        /**
         * Sort elements on chunk.
         */
        uint sortChunk(uint index);

        /**
         * Remove chunk.
         */
        void removeChunk(uint index);

        /**
         * Mege two chunks.
         */
        bool mergeChunks(uint *iA, uint *iB, uint tid);

        /**
         * To be used as a thread.
         */
        static void workerMerge(void *ptr);

        /**
         * Load state from disk.
         */
        bool loadState(uint *i, uint *count);

        /**
         * Verify if full_text[index] matches substr.
         */
        bool matchFound(uint index, uchar *substr);

        /**
         * Binary search list, looking for substr on list, bounded by l and r.
         */
        uint binarySearch(uint *list, uint l, uint r, uchar *substr);

        /**
         * Take a pick on index. Print just a few chars from that position.
         */
        void pick(uint index);
};

struct ThreadType {
    SuffixArray *obj;
    uint i;
    uint tid;
};

#endif
