#ifndef SEARCHALG_H
#define SEARCHALG_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

class StringNode {
    public:
        unsigned char *s;
        StringNode *next;
};

class SearchAlg {
    public:
        virtual bool loadFile (char *filename) = 0;
        virtual unsigned char **search(unsigned char *substr) = 0;
        unsigned char *getFullText(char *filename);
        void insertionSort(unsigned int *list, unsigned int size);

    protected:
        unsigned char *full_text;


};

#endif
