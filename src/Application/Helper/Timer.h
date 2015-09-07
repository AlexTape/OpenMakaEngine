#ifndef OPENMAKAENGINE_TIMER_H
#define OPENMAKAENGINE_TIMER_H

#include <time.h>

namespace om {

    class Timer {

    private:

        clock_t time;

    public:

        Timer(void);

        virtual ~Timer(void);

        double getMillis();

        void restart();
    };

};

#endif //OPENMAKAENGINE_TIMER_H
