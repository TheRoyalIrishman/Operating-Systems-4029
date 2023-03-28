#pragma once

#include <iostream>
#include <chrono>

using namespace std;
using namespace chrono;

typedef high_resolution_clock Clock;

void clockFunction() {
    auto timeOne = Clock::now();
    auto timeTwo = Clock::now();

    cout << duration_cast<nanoseconds>(timeTwo - timeOne).count() << " nanoseconds" << endl;
}