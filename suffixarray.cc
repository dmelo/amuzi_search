#include "suffixarray.h"

/**
 * Swap two elements on array.
 */
void SuffixArray::swap(uint i, uint j)
{
    uint aux = array[i];
    array[i] = array[j];
    array[j] = aux;
}

SuffixArray::SuffixArray()
{
    setCmpDebug(false);
}

/**
 * Calculate the filename for the a given index.
 */
char *SuffixArray::getFilename(uint index)
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
uint SuffixArray::writeChunk(uint *indexFile)
{
    FILE *fd;
    char *filename;
    uint i, ret;

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

uint *SuffixArray::readChunk(uint index)
{
    uint *chunk = NULL, aux, i;
    FILE *fd;
    char *filename;

    filename = getFilename(index);
    if ((fd = fopen(filename, "r")) != NULL) {
        chunk = (uint *) malloc(sizeof(uint) * CHUNK_SIZE);
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
    } else {
        printf("Error: unable to open file %s for reading\n", filename);
    }

    free(filename);

    return chunk;
}

uint SuffixArray::sortChunk(uint index)
{
    uint *chunk = readChunk(index), newIndex;

    quickSort(chunk, CHUNK_SIZE);
    removeChunk(index);
    return writeChunk(chunk);
}

void SuffixArray::removeChunk(uint index)
{
    char *filename;

    filename = getFilename(index);
    remove(filename);
    free(filename);
}

bool SuffixArray::mergeChunks(uint *iA, uint *iB)
{
    uint *chunkA, *chunkB, i;
    bool ret = false;

    chunkA = readChunk(*iA);
    chunkB = readChunk(*iB);


    if (NULL != chunkA && NULL != chunkB) {
        removeChunk(*iA);
        removeChunk(*iB);

        for (i = 0; i < CHUNK_SIZE; i++) {
            chunkTmp[i] = chunkA[i];
            chunkTmp[i + CHUNK_SIZE] = chunkB[i];
        }

        SORT_FUNCTION(chunkTmp, 2 * CHUNK_SIZE);
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
    uint i, j, count, arrayChunk[CHUNK_SIZE], auxA, auxB, total;

    printf("Loading file %s for suffix array.\n", filename);
    if (getFullText(filename)) {
        size = strlen((const char *) full_text);
        array = (uint *) malloc(((size / CHUNK_SIZE) + 1) * sizeof(uint));
        count = 0;
        for (i = 0; i < size; i += CHUNK_SIZE) {
            for (j = 0; j < CHUNK_SIZE; j++) {
                arrayChunk[j] = i + j;
            }
            array[count] = writeChunk(arrayChunk);
            count++;
        }

        timer t;
        total = 0;
        t.start();
        if (PRESORT_CHUNKS) {
            printf("start presoring chunks\n");
            for (i = 0; i < count; i++) {
                array[i] = sortChunk(array[i]);
            }
            printf("chunks presorted\n");
        }

        for (i = 0; i < count - 1; i++) {
            for (j = 0; j < count - 1 - i; j++) {
                total++;
                auxA = array[j];
                auxB = array[j + 1];
                mergeChunks(&auxA, &auxB);
                array[j] = auxA;
                array[j + 1] = auxB;

            }
        }
        t.end();
        printf("merge %u chunks %s\n", total, t.toString());
    }
}

uchar **SuffixArray::search(uchar *substr)
{
    // TODO: search for substr on the suffix array.
}
