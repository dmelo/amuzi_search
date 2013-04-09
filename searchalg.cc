#include"searchalg.h"

bool SearchAlg::getFullText(char *filename) {
    FILE *fd;
    int fread_ret;

    if ((fd = fopen(filename, "rb")) != NULL) {
        fseek(fd, 0, SEEK_END);
        size = (uint) ftell(fd);
        fseek(fd, 0, SEEK_SET);
        full_text = (uchar *) malloc((size + 1) * sizeof(uchar));
        if (size != (fread_ret = fread(full_text, sizeof(uchar), size, fd))) {
            printf("fread returned %d while %d was expected.\n", fread_ret, size);
            free(full_text);
            full_text = NULL;
        } else {
            fclose(fd);
        }
        return true;
    } else {
        printf("fopen returned NULL.\n");
        full_text = NULL;
        return false;
    }
}

void SearchAlg::insertionSort(uint *list, uint size)
{
    uint holePos, valueToInsert, i;

    for (i = 1; i < size; i++) {
        valueToInsert = list[i];
        holePos = i;

        while(holePos > 0 && 
            cmp(valueToInsert, list[holePos - 1]) < 0
        ) {
            list[holePos] = list[holePos - 1];
            holePos--;
        }
        list[holePos] = valueToInsert;
    }
}

uint SearchAlg::quickSortPartition(uint *list, uint l, uint r)
{
    uint pivotIndex = (l + r) / 2,
         pivotValue = list[pivotIndex],
         aux, i,
         storeIndex = l;

    aux = list[pivotIndex];
    list[pivotIndex] = list[r];
    list[r] = aux;

    for (i = l; i < r; i++) {
        if (cmp(list[i], pivotValue) < 0) {
            aux = list[storeIndex];
            list[storeIndex] = list[i];
            list[i] = aux;
            storeIndex++;
        }
    }

    aux = list[storeIndex];
    list[storeIndex] = list[r];
    list[r] = aux;

    return storeIndex;
}

void SearchAlg::quickSortInternal(uint *list, uint l, uint r)
{
    uint j;
    if (l < r) {
        j = quickSortPartition(list, l, r);
        if (l < j) {
            quickSortInternal(list, l, j - 1);
        }

        if (j < r && UINT_MAX != list[j]) {
            quickSortInternal(list, j + 1, r);
        }
    }
}

void SearchAlg::quickSort(uint *list, uint size)
{
    quickSortInternal(list, 0, size - 1);
}

void SearchAlg::mergeSort(uint *list, uint size)
{
    uint listAux[size], iA = 0, iB = 0, count = 0, size_2 = size / 2, i;

    while (iA < size_2 || iB < size_2) {
        if (iA < size_2 && iB < size_2) {
            if (cmp(list[iA], list[iB + size_2]) < 0) {
                listAux[count] = list[iA];
                iA++;
            } else {
                listAux[count] = list[iB + size_2];
                iB++;
            }
        } else if (iA < size_2) {
            listAux[count] = list[iA];
            iA++;
        } else if (iB < size_2) {
            listAux[count] = list[iB + size_2];
            iB++;
        }

        count++;
    }

    for (i = 0; i < size; i++) {
        list[i] = listAux[i];
    }
}

void SearchAlg::bfSort(uint *list, uint size)
{
    uint i, j, aux;

    for (i = 0; i < size; i++) {
        for (j = i + 1; j < size; j++) {
            if (cmp(list[i], list[j]) > 0) {
                aux = list[i];
                list[i] = list[j];
                list[j] = aux;
            }
        }
    }
}

int SearchAlg::cmp(uint a, uint b)
{
    if (cmpDebug) {
        if (a < b) {
            return -1;
        } else if (a == b) {
            return 0;
        } else {
            return 1;
        }
    } else {
        if (UINT_MAX == a) {
            return 1;
        } else if (UINT_MAX == b) {
            return -1;
        } else {
            return strcmp((char *) &full_text[a], (char *) &full_text[b]);
        }
    }
}

void SearchAlg::setCmpDebug(bool cmpDebug)
{
    this->cmpDebug = cmpDebug;
}

uchar *SearchAlg::getResult(uint i)
{
    uint start, end, j;
    uchar *ret = NULL;

    for(start = i; '\n' != full_text[start] && start >= 0; start--);
    start++;
    for(end = i; '\n' != full_text[end] && end < size; end++);
    ret = (uchar *) malloc((end - start + 1) * sizeof(uchar));
    for (j = 0; j < end - start; j++) {
        ret[j] = full_text[start + j];
    }
    ret[j] = '\0';

    return ret;
}
