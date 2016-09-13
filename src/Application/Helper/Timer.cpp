#pragma once
#ifndef OPENMAKAENGINE_TIMER_CPP
#define OPENMAKAENGINE_TIMER_CPP

#include <iostream>

#include "Timer.h"
#include "../Controller.h"

using namespace std;
using namespace om;

Timer::Timer(void) {

    if (Controller::MODE_DEBUG) {
        cout << "Creating Timer instance.." << endl;
    }

    time = clock();

}

Timer::~Timer(void) {
    if (Controller::MODE_DEBUG) {
        cout << "Deleting Timer instance.." << endl;
    }
}

void Timer::restart() {
    time = clock();
}

double Timer::getMillis() {
    double diff = clock() - time;
    return (diff) / (CLOCKS_PER_SEC / 1000);
}

#endif
