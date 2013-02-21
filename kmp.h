#ifndef KMP_H
#define KMP_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "timer.h"

class KMP {
    public:
        KMP();
        bool load_file(char *filename);
        char **search(char *substr, unsigned int limit);

    private:
        char *full_text;
        unsigned int size;
};

#endif
