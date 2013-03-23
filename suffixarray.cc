#include "suffixarray.h"

void SuffixArray::swap(unsigned int i, unsigned int j)
{
    unsigned int aux = array[i];
    array[i] = array[j];
    array[j] = aux;
}

SuffixArray::SuffixArray()
{
}

bool SuffixArray::recordPieceFile(unsigned int indexFile[S_PER_FILE])
{
    FILE *fd;
    char filename[1024];
    unsigned int i;
    bool ret = false;

    sprintf(filename, "data/%u.array", indexFile[S_PER_FILE / 2]);
    if ((fd = fopen(filename, "w")) != NULL) {
        for (i = 0; i < S_PER_FILE; i++) {
            fprintf(fd, "%u\n", indexFile[i]);
        }

        fclose(fd);

        ret = true;
    }
    
    return ret;
}

bool SuffixArray::loadFile(char *filename)
{
    unsigned int i, j, aux, index = 0, 
                 leastIndex = 0, // Points to the least substring.
                 indexFile[S_PER_FILE],
                 arrayIndex = 0, // Points to the latest array index filled
                 arraySize,
                 countDown;

    printf("Loading file %s for suffix array.\n", filename);
    if ((full_text = getFullText(filename)) != NULL) {
        size = strlen((const char *) full_text);
        countDown = size;
        arraySize = (size / S_PER_FILE) + 1;
        array = (unsigned int *) malloc(sizeof(unsigned int) * arraySize);

        // initialize array.
        for (i = 0; i < arraySize; i++) {
            array[i] = i;
        }

        // initialize leastIndex;
        for (i = 0; i < size; i++) {
            if (strcmp((char *) &full_text[i], (char *) &full_text[leastIndex]) < 0) {
                leastIndex = i;
            }
        }

        while (countDown > 0) {
            memset(indexFile, 0, S_PER_FILE * sizeof(unsigned int));
            for (i = 0; i < S_PER_FILE; i++) {
                index = 0;
                for (j = 0; j < size; j++) {
                    if (strcmp((char *) &full_text[j], (char *) &full_text[index]) < 0 &&
                            strcmp((char *) &full_text[j], (char *) &full_text[leastIndex]) > 0) {
                        index = j;
                    }
                }
                indexFile[i] = index;
                printf("-- %d\n", i);
                leastIndex = index;
                if (countDown-- <= 0) {
                    break;
                }
            }
            array[arrayIndex++] = indexFile[S_PER_FILE / 2];

            recordPieceFile(indexFile);
        }
    }
}

unsigned char **SuffixArray::search(unsigned char *substr)
{
    // TODO: search for substr on the suffix array.
    
}
