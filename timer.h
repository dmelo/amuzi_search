#ifndef TIMER_H
#define TIMER_H
#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<iostream>
#include<string>

using namespace std;

class timer {
    private:
        struct timezone tz;
        bool started;
        bool ended;
        struct timeval st;
        struct timeval en;
        char ret[60];
    public:
        timer();
        void restart();
        bool start();
        bool end();
        struct timeval getDiff();
        char *toString();
};

#endif // TIMER_H
