#include "kmp.h"

KMP::KMP()
{
    full_text = NULL;
    size = 0;
}

/**
 * Every line from the file must have the following format:
 * ID$$TEXT, being id a number and TEXT the searchable text.
 */
bool KMP::load_file(char *filename)
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

char **KMP::search(char *substr, unsigned int limit)
{
    timer t;
    t.start();
    unsigned int i, j, sub_size, start, end, n_found = 0;
    char **ret = (char **) calloc(limit, sizeof(char *));

    sub_size = strlen(substr);
    for (i = 0; i < size && n_found < limit; i++) {
        for (j = 0; full_text[i + j] == substr[j] && j < sub_size; j++);
        if (sub_size == j) { // We have a match.
            printf("match found at %d\n", i);
            for(start = i; '\n' != full_text[start] && start >= 0; start--);
            if (0 != start) {
                start++;
            }
            for(end = i; '\n' != full_text[end] && end < size; end++);
            ret[n_found] = (char *) malloc((end - start + 2) * sizeof(char));
            for (j = 0; j <= end - start; j++) {
                ret[n_found][j] = full_text[start + j];
            }
            ret[j] = '\0';
            n_found++;
        }
    }

    t.end();
    printf("search %s %u -> %s\n", substr, limit, t.toString());

    return ret;
}
