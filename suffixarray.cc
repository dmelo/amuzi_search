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
    lock = PTHREAD_MUTEX_INITIALIZER;
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

void SuffixArray::readChunk(uint index, uint varIndex, uint tid)
{
    uint *chunk = NULL, aux, i;
    FILE *fd;
    char filename[1024];

    getFilename(index, filename);
    if ((fd = fopen(filename, "rb")) != NULL) {
        if (0 == varIndex) {
            fread(chunkA[tid], CHUNK_SIZE, sizeof(uint), fd);
        } else {
            fread(chunkB[tid], CHUNK_SIZE, sizeof(uint), fd);
        }
        fclose(fd);
    } else {
        printf("Error: unable to open file %s for reading\n", filename);
    }
}

uint SuffixArray::sortChunk(uint index)
{
    readChunk(index, 0, 0);
    quickSort(chunkA[0], CHUNK_SIZE);
    removeChunk(index);

    return writeChunk(chunkA[0]);
}

void SuffixArray::removeChunk(uint index)
{
    char filename[1024];

    getFilename(index, filename);
    remove(filename);
}

bool SuffixArray::mergeChunks(uint *iA, uint *iB, uint tid)
{
    uint i;
    bool ret = false;

    readChunk(*iA, 0, tid);
    readChunk(*iB, 1, tid);


    removeChunk(*iA);
    removeChunk(*iB);

    memcpy(chunkTmp[tid], chunkA[tid], CHUNK_SIZE * sizeof(uint));
    memcpy(&(chunkTmp[tid][CHUNK_SIZE]), chunkB[tid], CHUNK_SIZE * sizeof(uint));

    SORT_FUNCTION(chunkTmp[tid], 2 * CHUNK_SIZE);
    *iA = writeChunk(chunkTmp[tid]);
    *iB = writeChunk(&(chunkTmp[tid][CHUNK_SIZE]));

    //pthread_mutex_lock(&lock);
    total++;
    //pthread_mutex_unlock(&lock);

    return true;
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

void SuffixArray::workerMerge(void *ptr)
{
    ThreadType *t = (ThreadType *) ptr;
    uint auxA, auxB, i = t->i, j, tid = t->tid;

    for (j = i + N_THREAD; j < t->obj->count; j += N_THREAD) {
        auxA = t->obj->array[i];
        auxB = t->obj->array[j];
        t->obj->mergeChunks(&auxA, &auxB, tid);
        t->obj->array[i] = auxA;
        t->obj->array[j] = auxB;
    }
}

bool SuffixArray::loadFile(char *filename)
{
    uint i, j, k, auxA, auxB;
    FILE *fd;
    pthread_t th[N_THREAD];
    ThreadType ta[N_THREAD];

    printf("Loading file %s for suffix array.\n", filename);
    fflush(stdout);
    if (getFullText(filename)) {
        // Allocate memory for suffix array.
        array = (uint *) malloc(((size / CHUNK_SIZE) + 1) * sizeof(uint));
        timer t;

        if (!loadState(&i, &count)) {
            count = 0;
            for (i = 0; i < size; i += CHUNK_SIZE) {
                for (j = 0; j < CHUNK_SIZE; j++) {
                    chunkA[0][j] = i + j < size ? i + j : UINT_MAX;
                }
                array[count] = writeChunk(chunkA[0]);
                count++;
            }

            total = 0;
            t.start();
            if (PRESORT_CHUNKS) {
                printf("start presoring chunks\n");
                fflush(stdout);
                for (i = 0; i < count; i++) {
                    array[i] = sortChunk(array[i]);
                }
                printf("chunks presorted\n");
                fflush(stdout);
            }
            
            i = 0;
        }

        for (i; i < count; i++) {;
            timer t2;
            t2.start();
            uint total0 = total;

            for (j = 0; j < N_THREAD; j++) {
                ta[j].obj = this;
                ta[j].i = i + j;
                ta[j].tid = j;

                pthread_create(&th[j], NULL, (void* (*)(void*)) &SuffixArray::workerMerge, static_cast<void *>(&ta[j]));
            }


            for (j = 0; j < N_THREAD; j++) {
                pthread_join(th[j], NULL);
            }

            for (j = 0; j < N_THREAD && i + j < count; j++) {
                for (k = j + 1; k < N_THREAD && i + k < count; k++) {
                    auxA = array[i + j];
                    auxB = array[i + k];
                    mergeChunks(&auxA, &auxB, 0);
                    array[i + j] = auxA;
                    array[i + k] = auxB;
                }
            }


            saveState(i + 1, count, total);
            t2.end();
            printf("%u merges in %s\n", total - total0, t2.toString());
        }

        t.end();
        printf("merge %u chunks %s\n\n", total, t.toString());
        fflush(stdout);
        fflush(stderr);
    }
}

/**
 * Test if pattern matches full_text on index position.
 *
 * @param uint index full_text index to be tested.
 * @param char *substr pattern to be found.
 *
 * @return bool Return true if patter matches, false otherwise.
 */
bool SuffixArray::matchFound(uint index, uchar *substr)
{
    uint len = strlen((const char *) substr), i;

    if (UINT_MAX == index) {
        return false;
    }

    for (i = 0; i < len; i++) {
        if (full_text[index + i] != substr[i]) {
            return false;
        }
    }

    return true;
}

void SuffixArray::pick(uint index)
{
    char p[15];
    uint i;
    for (i = 0; i < 14; i++) {
        p[i] = (char) full_text[index + i];
    }
    p[14] = '\0';

    printf("pick: %u -- %s\n", index, p);
}

uint SuffixArray::binarySearch(uint *list, uint l, uint r, uchar *substr)
{
    uint index = (l + r) / 2, ret;

    pick(list[index]);

    printf("binarySearch: %u %u\n", l, r);

    if (l == r || matchFound(list[index], substr)) {
        ret = index;
    } else {
        if (UINT_MAX == list[index] || strcmp(
            (char *) substr,
            (char *) &full_text[list[index]]
        ) < 0) {
            ret = binarySearch(list, l, index, substr);
        } else {
            if (index == l) {
                ret = binarySearch(list, index, r - 1, substr);
            } else {
                ret = binarySearch(list, index, r, substr);
            }
        }
    }

    return ret;
}

uchar **SuffixArray::search(uchar *substr)
{
    uchar **ret = (uchar **) calloc(SEARCH_LIMIT, sizeof(uchar *));
    timer t;
    t.start();
    printf("searching %s\n", (char *) substr);
    uint arraySize, index, i;

    arraySize = (size / CHUNK_SIZE) + 1;

    index = binarySearch(array, 0, arraySize - 1, substr);
    readChunk(array[index], 0, 0);

    index = binarySearch(chunkA[0], 0, CHUNK_SIZE - 1, substr);
    pick(chunkA[0][index]);

    while (index > 0 && matchFound(chunkA[0][index], substr)) {
        index--;
    }
    index++;

    for (i = 0; i < SEARCH_LIMIT && index + i < CHUNK_SIZE &&
        matchFound(chunkA[0][index + i], substr); i++) {
        ret[i] = getResult(chunkA[0][index + i]);
    }

    for (; i < SEARCH_LIMIT; i++) {
        ret[i] = NULL;
    }

    t.end();
    printf("--- TIME %s\n", t.toString());


    return ret;
}
