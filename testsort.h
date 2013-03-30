#ifndef TESTSORT_H
#define TESTSORT_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include "timer.h"
#include "searchalg.h"

#define TEST_SIZE 1024 * 64

class TestSort : public SearchAlg {
    public:
        TestSort();
        bool loadFile(char *filename);
        uchar **search(uchar *substr);
        void testAlgs();

    protected:
        uint *getRandom(uint size);
        bool assertList(uint *list);
        bool testQuickSort();
        bool testInsertionSort();
        bool testMergeSort();
        bool testBfSort();
};

#endif
