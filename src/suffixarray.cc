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
void SuffixArray::getFilename(uint index, bool pre, char *filename)
{
    sprintf(filename,
        pre ? "data/pre-%u.array" : "data/sar-%u.array",
        index
    );
}

/**
 * Write the chunk file, with uint and return the index used to name the chunk.
 * This methods assumes that indexFile have CHUNK_SIZE uint elements.
 */
uint SuffixArray::writeChunk(uint *indexFile, bool pre)
{
    FILE *fd;
    char filename[1024];
    uint i, ret;

    ret = indexFile[0];
    getFilename(ret, pre, filename);

    if ((fd = fopen(filename, "wb")) != NULL) {
        fwrite(indexFile, pre ? PRE_CHUNK_SIZE : CHUNK_SIZE, sizeof(uint), fd);
        fclose(fd);
    } else {
        printf("Countn't write Chunk %u\n", indexFile[0]);
        ret = UINT_MAX;
    }

    return ret;
}

void SuffixArray::readChunk(uint index, bool pre)
{
    uint aux, i;
    FILE *fd;
    char filename[1024];

    getFilename(index, pre, filename);
    if ((fd = fopen(filename, "rb")) != NULL) {
        fread(chunk, pre ? PRE_CHUNK_SIZE : CHUNK_SIZE, sizeof(uint), fd);
        fclose(fd);
    } else {
        printf("Error: unable to open file %s for reading\n", filename);
    }
}

uint SuffixArray::sortPreChunk(uint index)
{
    readChunk(index, true);
    quickSort(chunk, PRE_CHUNK_SIZE);
    removeChunk(index, true);

    return writeChunk(chunk, true);
}

void SuffixArray::removeChunk(uint index, bool pre)
{
    char filename[1024];

    getFilename(index, pre, filename);
    remove(filename);
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

void SuffixArray::initQueues()
{
    q.queueSize = MEM_CHUNKS / (count > 0 ? count * sizeof(uint) : sizeof(uint));
    q.queueSize = q.queueSize > PRE_CHUNK_SIZE ?
        PRE_CHUNK_SIZE : q.queueSize;
    q.queueNum = size > 0 ? (size / PRE_CHUNK_SIZE) + 1 : 0;

    q.queuePos = (uint *) calloc(q.queueSize, sizeof(uint));
    q.queueLimit = (uint *) calloc(q.queueSize, sizeof(uint));
    q.queueExtPos = (uint *) calloc(q.queueSize, sizeof(uint));
    q.queues = (uint **) malloc(q.queueNum * sizeof(uint*));
    for (uint i = 0; i < q.queueNum; i++) {
        q.queues[i] = (uint *) malloc(q.queueSize * sizeof(uint));
    }

    loadAllQueues();
}

void SuffixArray::loadQueue(uint index)
{
    char filename[1024];
    uint nRead;
    FILE *fd;

    getFilename(preArray[index], true, filename);
    fd = fopen(filename, "rb");
    fseek(fd, q.queueExtPos[index] * sizeof(uint), SEEK_SET);
    nRead = fread(q.queues[index], sizeof(uint), q.queueSize, fd);
    printf("loadQueue %u. tried to read %u uints. read %u\n", index, q.queueSize, nRead);
    fclose(fd);
    q.queuePos[index] = 0;
    q.queueLimit[index] = nRead;
    q.queueExtPos[index] += nRead;
}

void SuffixArray::loadAllQueues()
{
    for (uint i = 0; i < q.queueNum; i++) {
        loadQueue(i);
    }
}

uint SuffixArray::mergeQueue()
{
    uint i,
         minValue = UINT_MAX,
         minQueue = 0;

    for(i = 0; i < q.queueNum; i++) {
        /* printf("-- pos %u. limit %u. v0 %u. min %u.\n",
            q.queuePos[i],
            q.queueLimit[i],
            q.queues[i][q.queuePos[i]],
            minValue
        );*/
        if (q.queuePos[i] < q.queueLimit[i]
            && (UINT_MAX == minValue
                || cmp(q.queues[i][q.queuePos[i]], minValue) < 0)) {
            minValue = q.queues[i][q.queuePos[i]];
            minQueue = i;
        }
    }

    if (UINT_MAX != minValue) {
        q.queuePos[minQueue]++;
        if (q.queuePos[minQueue] >= q.queueLimit[minQueue]
            && q.queueExtPos[minQueue] < PRE_CHUNK_SIZE) {
            loadQueue(minQueue);
        }
    }

    //printf("minValue = %u. minQueue = %u\n", minValue, minQueue);

    return minValue;
}

bool SuffixArray::loadFile(char *filename)
{
    uint i, j, k, auxA, auxB;
    FILE *fd;


    printf("Loading file %s for suffix array.\n", filename);
    fflush(stdout);
    if (getFullText(filename)) {
        // Allocate memory for suffix array.
        array = (uint *) malloc(((size / CHUNK_SIZE) + 1) * sizeof(uint));

        if (!loadState(&i, &count)) {
            printf("preArray will have %u %u - %u uints\n",
                size, PRE_CHUNK_SIZE, (size / PRE_CHUNK_SIZE) + 1u);
            preArray = (uint *) malloc(
                ((size / PRE_CHUNK_SIZE) + 1) * sizeof(uint)
            );

            count = 0;
            for (i = 0; i < size; i += PRE_CHUNK_SIZE) {
                for (j = 0; j < PRE_CHUNK_SIZE; j++) {
                    chunk[j] = i + j < size ? i + j : UINT_MAX;
                }
                preArray[count] = writeChunk(chunk, true);
                count++;
            }

            total = 0;
            printf("start presoring chunks\n");
            fflush(stdout);
            for (i = 0; i < count; i++) {
                printf("sorting preChunk %u -- %u\n", i, preArray[i]);
                preArray[i] = sortPreChunk(preArray[i]);
            }
            printf("chunks presorted\n");
            fflush(stdout);
            
            i = 0;

            initQueues();

            

            for (i = 0; i < size; i += CHUNK_SIZE) {
                for (j = 0; j < CHUNK_SIZE; j++) {
                    chunk[j] = mergeQueue();
                }
                printf("writing chunk %u of %u\n", i, size);
                array[total] = writeChunk(chunk, false);
                total++;
            }

            saveState(i, total, 0);
        }

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

    if (UINT_MAX == index || index + len >= size) {
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
    for (i = 0; i < 14 && i + index < size; i++) {
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
    uint arraySize, index, i = 0;

    if (size > 0) {
        arraySize = (size / CHUNK_SIZE) + 1;

        index = binarySearch(array, 0, arraySize - 1, substr);
        readChunk(array[index], false);

        index = binarySearch(chunk, 0, CHUNK_SIZE - 1, substr);
        pick(chunk[index]);

        while (index > 0 && matchFound(chunk[index], substr)) {
            index--;
        }
        index++;


        for (; i < SEARCH_LIMIT && index + i < CHUNK_SIZE &&
            matchFound(chunk[index + i], substr); i++) {
            ret[i] = getResult(chunk[index + i]);
        }
    }

    for (; i < SEARCH_LIMIT; i++) {
        ret[i] = NULL;
    }

    t.end();
    printf("--- TIME %s\n", t.toString());


    return ret;
}
