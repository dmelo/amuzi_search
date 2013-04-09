#ifndef SUFFIXARRAY_H
#define SUFFIXARRAY_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include<stdint.h>
#include "timer.h"
#include "searchalg.h"

#define SORT_FUNCTION mergeSort

struct ExternalSorting {
    uint **queues, // An array of queues
         queueSize, // The size/depth of each queue
         queueNum, // Number of queues, equals to the number of preChunks
         *queuePos, // Internal position on each queue
         *queueLimit, // The end of each queue.
         *queueExtPos; // absolute position on the file.
};



class SuffixArray : public SearchAlg {
    public:
        uint *array, count, total, *preArray;

        SuffixArray();
        bool loadFile(char *filename);
        uchar **search(uchar *substr);

        /**
         * Save current state on disk.
         */
        bool saveState(uint i, uint count, uint total);


    private:
        uint chunk[CHUNK_SIZE > PRE_CHUNK_SIZE ? CHUNK_SIZE : PRE_CHUNK_SIZE];
        ExternalSorting q;

        /**
         * Swap two positions on array.
         */
        void swap(uint i, uint j);

        /**
         * Calculate the file name.
         */
        void getFilename(uint index, bool pre, char *filename);

        /**
         * Write chunk on file.
         */
        uint writeChunk(uint *indexFile, bool pre);

        /**
         * Read chunk from file.
         */
        void readChunk(uint index, bool pre);

        /**
         * Sort elements on chunk.
         */
        uint sortPreChunk(uint index);

        /**
         * Remove chunk.
         */
        void removeChunk(uint index, bool pre);

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


        void initQueues();
        void loadQueue(uint index);
        void loadAllQueues();
        uint mergeQueue();
};

#endif
