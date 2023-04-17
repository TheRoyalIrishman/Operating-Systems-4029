#pragma once

#include <iostream>
#include <chrono>

using namespace std;
using namespace chrono;

typedef high_resolution_clock Clock;

void clockDelayFunction(int ms) {
    auto timeOne = Clock::now();
    auto timeTwo = Clock::now();

    // cout << duration_cast<nanoseconds>(timeTwo - timeOne).count() << " nanoseconds" << endl;

    // return duration_cast<nanoseconds>(timeTwo - timeOne).count();

    while (duration_cast<milliseconds>(timeTwo - timeOne).count() < ms) {
        timeTwo = Clock::now();
    }
}