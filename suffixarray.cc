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
char *SuffixArray::getFilename(uint index, char *filename)
{
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
    char filename[1024];
    uint i, ret;

    ret = indexFile[0];
    getFilename(ret, filename);

    if ((fd = fopen(filename, "wb")) != NULL) {
        fwrite(indexFile, CHUNK_SIZE, sizeof(uint), fd);
        fclose(fd);
    } else {
        ret = UINT_MAX;
    }

    return ret;
}

void SuffixArray::readChunk(uint index, uint varIndex)
{
    uint *chunk = NULL, aux, i;
    FILE *fd;
    char filename[1024];

    getFilename(index, filename);
    if ((fd = fopen(filename, "rb")) != NULL) {
        if (0 == varIndex) {
            fread(chunkA, CHUNK_SIZE, sizeof(uint), fd);
        } else {
            fread(chunkB, CHUNK_SIZE, sizeof(uint), fd);
        }
        fclose(fd);
    } else {
        printf("Error: unable to open file %s for reading\n", filename);
    }
}

uint SuffixArray::sortChunk(uint index)
{
    readChunk(index, 0);
    quickSort(chunkA, CHUNK_SIZE);
    removeChunk(index);

    return writeChunk(chunkA);
}

void SuffixArray::removeChunk(uint index)
{
    char filename[1024];

    getFilename(index, filename);
    remove(filename);
}

bool SuffixArray::mergeChunks(uint *iA, uint *iB)
{
    uint i;
    bool ret = false;

    readChunk(*iA, 0);
    readChunk(*iB, 1);


    if (NULL != chunkA && NULL != chunkB) {
        removeChunk(*iA);
        removeChunk(*iB);

        memcpy(chunkTmp, chunkA, CHUNK_SIZE * sizeof(uint));
        memcpy(&chunkTmp[CHUNK_SIZE], chunkB, CHUNK_SIZE * sizeof(uint));

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

bool SuffixArray::saveState(uint i, uint count, uint total)
{
    FILE *fd;
    uint a;
    bool ret;

    if ((fd = fopen("data/info.txt", "w")) != NULL) {
        fprintf(fd, "%u %u\n", i, count);
        for (a = 0; a < count; a++) {
            fprintf(fd, "%u\n", array[a]);
        }
        fclose(fd);
        printf("completed %u of %u\n", total, count * count / 2);
        ret = true;
    } else {
        ret = false;
    }

    return ret;
}

bool SuffixArray::loadState(uint *i, uint *count)
{
    FILE *fd;
    uint a;
    bool ret;

    if ((fd = fopen("data/info.txt", "r")) != NULL) {
        fscanf(fd, " %u %u ", i, count);
        for (a = 0; a < *count; a++) {
            fscanf(fd, " %u ", &array[a]);
        }
        fclose(fd);
        ret = true;
        printf("loadState %u %u\narray: ", *i, *count);
        for (a = 0; a < *count; a++) {
            printf("%u ", array[a]);
        }
        printf("\n");
        ret = true;
    } else {
        ret = false;
    }

    printf("loadState returned %s\n", ret ? "true" : "false");
    return ret;
}

bool SuffixArray::loadFile(char *filename)
{
    uint i, j, k, count, auxA, auxB, total;
    FILE *fd;

    printf("Loading file %s for suffix array.\n", filename);
    if (getFullText(filename)) {
        // Allocate memory for suffix array.
        size = strlen((const char *) full_text);
        array = (uint *) malloc(((size / CHUNK_SIZE) + 1) * sizeof(uint));
        timer t, t2;

        if (!loadState(&i, &count)) {
            count = 0;
            for (i = 0; i < size; i += CHUNK_SIZE) {
                for (j = 0; j < CHUNK_SIZE; j++) {
                    chunkA[j] = i + j < size ? i + j : UINT_MAX;
                }
                array[count] = writeChunk(chunkA);
                count++;
            }

            total = 0;
            t.start();
            if (PRESORT_CHUNKS) {
                printf("start presoring chunks\n");
                for (i = 0; i < count; i++) {
                    array[i] = sortChunk(array[i]);
                }
                printf("chunks presorted\n");
            }
            
            i = 0;
        }

        for (i; i < count - 1; i++) {
            t2.start();
            for (j = i + 1; j < count; j++) {
                total++;
                auxA = array[i];
                auxB = array[j];
                mergeChunks(&auxA, &auxB);
                array[i] = auxA;
                array[j] = auxB;

            }
            t2.end();

            saveState(i + 1, count, total);
        }

        t.end();
        printf("merge %u chunks %s\n", total, t.toString());
    }
}

uchar **SuffixArray::search(uchar *substr)
{
    // TODO: search for substr on the suffix array.
}
