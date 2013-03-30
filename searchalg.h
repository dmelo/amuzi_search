#ifndef SEARCHALG_H
#define SEARCHALG_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include"searchalg.h"
#include"defs.h"

class StringNode {
    public:
        uchar *s;
        StringNode *next;
};

class SearchAlg {
    public:
        virtual bool loadFile (char *filename) = 0;
        virtual uchar **search(uchar *substr) = 0;
        bool getFullText(char *filename);
        void insertionSort(uint *list, uint size);
        void quickSort(uint *list, uint size);
        void mergeSort(uint *list, uint size);
        void bfSort(uint *list, uint size);

    protected:
        uchar *full_text;
        bool cmpDebug;

        void quickSortInternal(uint *list, uint l, uint r);
        uint quickSortPartition(uint *list, uint l, uint r);

        int cmp(uint a, uint b);
        void setCmpDebug(bool cmpDebug);


};

#endif
