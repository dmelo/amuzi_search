#include"searchalg.h"

unsigned char *SearchAlg::getFullText(char *filename) {
    FILE *fd;
    unsigned char *fullText;
    int size, fread_ret;

    if ((fd = fopen(filename, "rb")) != NULL) {
        fseek(fd, 0, SEEK_END);
        size = ftell(fd);
        fseek(fd, 0, SEEK_SET);
        fullText = (unsigned char *) malloc(size + 1);
        if (size != (fread_ret = fread(fullText, sizeof(unsigned char), size, fd))) {
            printf("fread returned %d while %d was expected.\n", fread_ret, size);
            free(fullText);
            fullText = NULL;
        } else {
            fclose(fd);
        }
    } else {
        printf("fopen returned NULL.\n");
        fullText = NULL;
    }

    return fullText;
}
