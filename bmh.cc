#include "bmh.h"

BMH::BMH()
{
    full_text = NULL;
    size = 0;
};

/**
 * Every line from the file must have the following format:
 * ID$$TEXT, being id a number and TEXT the searchable text.
 */
bool BMH::load_file(char *filename)
{
    timer t;
    t.start();

    FILE *fd;
    bool ret;

    if ((fd = fopen(filename, "rb")) != NULL) {
        fseek(fd, 0, SEEK_END);
        size = ftell(fd);
        fseek(fd, 0, SEEK_SET);
        full_text = (char *) malloc(size + 1);
        if (size != fread(full_text, sizeof(char), size, fd)) {
            free(full_text);
            ret = false;
        } else {
            fclose(fd);
            ret = true;
        }
    } else {
        ret = false;
    }

    t.end();
    printf("load_file %s %d -> %s\n", filename, size, t.toString());

    return ret;
}

char **BMH::search(char *substr)
{
    timer t;
    t.start();
    unsigned int i, j, sub_size, start, end, n_found = 0, aux, stringListIndexI;
    char **ret = (char **) calloc(BHM_LIMIT, sizeof(char *));

    memset(stringList, 0, MAX_STRING_MAX_LENGTH * sizeof(StringNode*));

    sub_size = strlen(substr);
    for (i = 0; i < size && n_found < BHM_LIMIT; i++) {
        for (j = 0; full_text[i + j] == substr[j] && j < sub_size; j++);
        if (sub_size == j) { // We have a match.
            for(start = i; '\n' != full_text[start] && start >= 0; start--);
            start++;
            for(end = i; '\n' != full_text[end] && end < size; end++);
            ret[n_found] = (char *) malloc((end - start + 1) * sizeof(char));
            for (j = 0; j < end - start; j++) {
                ret[n_found][j] = full_text[start + j];
            }
            ret[n_found][j] = '\0';
            n_found++;
        }
    }

    timer t2;
    t2.start();
    if (n_found > 0) {
        stringListIndexI = 0;
        memset(stringListIndex, 0, BHM_LIMIT * sizeof(StringNode *));
        for (i = 0; i < n_found; i++) {
            aux = strlen(ret[i]);
            snode = new StringNode();
            snode->s = ret[i];
            snode->next = NULL;
            stringListIndex[stringListIndexI] = snode;
            stringListIndexI++;
            if (NULL == stringList[aux]) {
                stringList[aux] = snode;
            } else {
                snodeAux = stringList[aux];
                while (NULL != snodeAux->next) {
                    snodeAux = snodeAux->next;
                }
                snodeAux->next = snode;
            }
        }

        memset(ret, 0, BHM_LIMIT * sizeof(char *));
        n_found = 0;
        for (i = 0; i < MAX_STRING_MAX_LENGTH; i++) {
            if (NULL != stringList[i]) {
                snodeAux = stringList[i];
                while (NULL != snodeAux) {
                    ret[n_found] = snodeAux->s;
                    snodeAux = snodeAux->next;
                    n_found++;
                }
            }
        }

        // Free StringNodes.
        for (i = 0; i < stringListIndexI; i++) {
            delete(stringListIndex[i]);
        }
    }
    t2.end();
    printf("sorting %d strings -> %s\n", n_found, t2.toString());

    t.end();
    printf("search %s %u -> %s\n", substr, BHM_LIMIT, t.toString());

    return ret;
}
