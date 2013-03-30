#include"timer.h"

timer::timer() {
    started = ended = false;
}

void timer::restart() {
    timer();
}

bool timer::start() {
    gettimeofday(&st, &tz);
    if(ended) {
        return false;
    }
    started = true;

    return true;
}

bool timer::end() {
    gettimeofday(&en, &tz);

    if(!started)
	return false;
    ended = true;

    return true;
}

struct timeval timer::getDiff() {
    struct timeval tv;

    tv.tv_sec = tv.tv_usec = 0;
    if(!started || ! ended)
	return tv;
    tv.tv_sec = en.tv_sec - st.tv_sec;
    tv.tv_usec = en.tv_usec - st.tv_usec;
    if(tv.tv_usec < 0) {
	tv.tv_sec--;
	tv.tv_usec = 1000 - tv.tv_usec;
    }

    return tv;
}

char *timer::toString() {
    struct timeval tv = getDiff();
    sprintf(ret, "timer - sec: %d. msec: %d. usec: %d.", (int) tv.tv_sec, 
	    (int) tv.tv_usec / 1000, 
	    (int) (tv.tv_usec - (1000 * (tv.tv_usec / 1000))));

    return ret;
}
