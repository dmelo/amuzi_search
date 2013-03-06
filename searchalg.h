#ifndef SEARCHALG_H
#define SEARCHALG_H

class StringNode {
    public:
        unsigned char *s;
        StringNode *next;
};

class SearchAlg {
    public:
        virtual bool loadFile (char *filename) = 0;
        virtual unsigned char **search(unsigned char *substr) = 0;
};

#endif
