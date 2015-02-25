#include "testsort.h"

TestSort::TestSort()
{
    srand(time(NULL));
}

bool TestSort::loadFile(char *filename)
{
    return false;
}

uchar **TestSort::search(uchar *substr)
{
    return NULL;
}

uint *TestSort::getRandom(uint size)
{
    uint *list = (uint *) malloc(sizeof(uint) * size), i, a, b, aux;

    for (i = 0; i < size; i++) {
        list[i] = i;
    }

    for (i = 0; i < size; i++) {
        a = rand() % size;
        b = rand() % size;
        aux = list[a];
        list[a] = list[b];
        list[b] = aux;
    }

    return list;
}

bool TestSort::assertList(uint *list)
{
    uint i;
    bool ret = true;

    for (i = 0; i < TEST_SIZE; i++) {
        if (list[i] != i) {
            printf("Failed asserting list[%d] == %d. %d found.\n", i, i, list[i]);
            ret = false;
            break;
        }
    }

    return ret;
}

bool TestSort::testQuickSort()
{
    bool ret;

    uint *list = getRandom(TEST_SIZE);
    quickSort(list, TEST_SIZE);
    ret = assertList(list);
    free(list);

    return ret;
}

bool TestSort::testInsertionSort()
{
    bool ret;

    uint *list = getRandom(TEST_SIZE);
    insertionSort(list, TEST_SIZE);
    ret = assertList(list);
    free(list);

    return ret;
}

bool TestSort::testMergeSort()
{
    bool ret;

    uint *list = getRandom(TEST_SIZE);
    quickSort(list, TEST_SIZE / 2);
    quickSort(&list[TEST_SIZE / 2], TEST_SIZE / 2);
    mergeSort(list, TEST_SIZE);
    ret = assertList(list);
    free(list);

    return ret;
}

bool TestSort::testBfSort()
{
    bool ret;

    uint *list = getRandom(TEST_SIZE);
    bfSort(list, TEST_SIZE);
    ret = assertList(list);
    free(list);

    return ret;
}



void TestSort::testAlgs()
{
    // TODO: this class is shit. reimplement with pointers to functions.
    setCmpDebug(true);
    if (testQuickSort()) {
        printf("Quicksort is good to go\n\n");
    } else {
        printf("Quicksort failed\n\n");
    }

    if (testInsertionSort()) {
        printf("InsertionSort is good to go\n\n");
    } else {
        printf("InsertionSort failed\n\n");
    }

    if (testMergeSort()) {
        printf("MergeSort is good to go\n\n");
    } else {
        printf("MergeSort failed\n\n");
    }

    if (testBfSort()) {
        printf("BfSort is good to go\n\n");
    } else {
        printf("BfSort failed\n\n");
    }
}
