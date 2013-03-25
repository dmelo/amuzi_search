#include "suffixarray.h"

/**
 * Swap two elements on array.
 */
void SuffixArray::swap(unsigned int i, unsigned int j)
{
    unsigned int aux = array[i];
    array[i] = array[j];
    array[j] = aux;
}

SuffixArray::SuffixArray()
{
}

/**
 * Calculate the filename for the a given index.
 */
char *SuffixArray::getFilename(unsigned int index)
{
    char *filename;

    filename = (char *) malloc(1024 * sizeof(char));
    sprintf(filename, "data/%u.array", index);

    return filename;
}

/**
 * Write the chunk file, with uint and return the index used to name the chunk.
 * This methods assumes that indexFile have CHUNK_SIZE uint elements.
 */
unsigned int SuffixArray::writeChunk(unsigned int *indexFile)
{
    FILE *fd;
    char *filename;
    unsigned int i, ret;

    ret = indexFile[0];
    filename = getFilename(ret);
    if ((fd = fopen(filename, "w")) != NULL) {
        for (i = 0; i < CHUNK_SIZE && indexFile[i] != UINT_MAX; i++) {
            fprintf(fd, "%u\n", indexFile[i]);
        }

        fclose(fd);
    } else {
        ret = UINT_MAX;
    }

    free(filename);
    
    return ret;
}

unsigned int *SuffixArray::readChunk(unsigned int index)
{
    unsigned int *chunk = NULL, aux, i;
    FILE *fd;
    char *filename;

    filename = getFilename(index);
    printf("try open %s\n", filename);
    if ((fd = fopen(filename, "r")) != NULL) {
        printf("opened %s\n", filename);
        chunk = (unsigned int *) malloc(sizeof(unsigned int) * CHUNK_SIZE);
        for (i = 0; i < CHUNK_SIZE && !feof(fd); i++) {
            fscanf(fd, " %u ", &aux);
            chunk[i] = aux;
        }
        fclose(fd);

        if (i < CHUNK_SIZE) { // chunk is not complete, may be the last chunk.
            for (; i < CHUNK_SIZE; i++) {
                chunk[i] = UINT_MAX;
            }
        }
    }
    free(filename);

    return chunk;
}

void SuffixArray::removeChunk(unsigned int index)
{
    char *filename;

    filename = getFilename(index);
    remove(filename);
    free(filename);
}

bool SuffixArray::mergeChunks(unsigned int *iA, unsigned int *iB)
{
    printf("merging %u and %u\n", *iA, *iB);
    unsigned int *chunkA, *chunkB, chunkTmp[CHUNK_SIZE * 2], i;
    bool ret = false;

    chunkA = readChunk(*iA);
    chunkB = readChunk(*iB);


    if (NULL != chunkA && NULL != chunkB) {
        printf("chunkA and chunkB are not null\n");
        removeChunk(*iA);
        removeChunk(*iB);

        for (i = 0; i < CHUNK_SIZE; i++) {
            chunkTmp[i] = chunkA[i];
            chunkTmp[i + CHUNK_SIZE] = chunkB[i];
        }

        insertionSort(chunkTmp, 2 * CHUNK_SIZE);
        *iA = writeChunk(chunkTmp);
        *iB = writeChunk(&chunkTmp[CHUNK_SIZE]);

        ret = true;
    } else {
        if (NULL == chunkA) {
            printf("chunkA is null\n");
        }
        if (NULL == chunkB) {
            printf("chunkB is null\n");
        }

    }

    return ret;
}

bool SuffixArray::loadFile(char *filename)
{
    unsigned int i, j, count, arrayChunk[CHUNK_SIZE], auxA, auxB;

    printf("Loading file %s for suffix array.\n", filename);
    if ((full_text = getFullText(filename)) != NULL) {
        size = strlen((const char *) full_text);
        array = (unsigned int *) malloc(((size / CHUNK_SIZE) + 1) * sizeof(unsigned int));
        count = 0;
        for (i = 0; i < size; i += CHUNK_SIZE) {
            for (j = 0; j < CHUNK_SIZE; j++) {
                arrayChunk[j] = i + j;
            }
            array[count] = writeChunk(arrayChunk);
            count++;
        }

        for (i = 0; i < count - 1; i++) {
            for (j = 0; j < count - 1 - i; j++) {
                auxA = j;
                auxB = j + 1;
                mergeChunks(&auxA, &auxB);
            }
        }
    }
}

unsigned char **SuffixArray::search(unsigned char *substr)
{
    // TODO: search for substr on the suffix array.
}
