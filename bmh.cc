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
bool BMH::loadFile(char *filename)
{
    timer t;
    t.start();

    FILE *fd;
    bool ret;

    if ((fd = fopen(filename, "rb")) != NULL) {
        fseek(fd, 0, SEEK_END);
        size = ftell(fd);
        fseek(fd, 0, SEEK_SET);
        full_text = (unsigned char *) malloc(size + 1);
        if (size != fread(full_text, sizeof(unsigned char), size, fd)) {
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

unsigned char **BMH::search(unsigned char *substr)
{
    timer t;
    t.start();
    unsigned int i, sub_size = strlen((char *) substr), start, end, n_found = 0, aux, stringListIndexI;
    unsigned char **ret = (unsigned char **) calloc(BMH_LIMIT, sizeof(unsigned char *));
    int j;

    memset(stringList, 0, MAX_STRING_MAX_LENGTH * sizeof(StringNode*));
    memset(bad_char_skip, 0, (UCHAR_MAX + 1) * sizeof(unsigned char));

    for (i = 0; i <= UCHAR_MAX; i++) {
        bad_char_skip[i] = sub_size;
    }

    for (i = 0; i < sub_size - 1; i++) {
        bad_char_skip[substr[i]] = sub_size - i - 1;
    }
    bad_char_skip[substr[sub_size - 1]] = 1;

    for (i = 0; i < size - sub_size && n_found < BMH_LIMIT;) {
        for (j = sub_size - 1; full_text[i + j] == substr[j] && j >= 0; j--);
        if (-1 == j) { // We have a match.
            for(start = i; '\n' != full_text[start] && start >= 0; start--);
            start++;
            for(end = i; '\n' != full_text[end] && end < size; end++);
            ret[n_found] = (unsigned char *) malloc((end - start + 1) * sizeof(unsigned char));
            for (j = 0; j < end - start; j++) {
                ret[n_found][j] = full_text[start + j];
            }
            ret[n_found][j] = '\0';
            n_found++;
            i++;
        }
        i += bad_char_skip[full_text[i + sub_size - 1]];
    }

    timer t2;
    t2.start();
    if (n_found > 0) {
        stringListIndexI = 0;
        memset(stringListIndex, 0, BMH_LIMIT * sizeof(StringNode *));
        for (i = 0; i < n_found; i++) {
            aux = strlen((char *) ret[i]);
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

        memset(ret, 0, BMH_LIMIT * sizeof(unsigned char *));
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
    printf("search %s %u -> %s\n", substr, BMH_LIMIT, t.toString());

    return ret;
}
