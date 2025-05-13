#include "timer.h"
#include <iostream>

void Timer::start() {
    start_time = std::chrono::high_resolution_clock::now();
    is_running = true;
}

void Timer::stop() {
    if (!is_running) {
        std::cerr << "Timer is not running!" << std::endl;
        return;
    }
    end_time = std::chrono::high_resolution_clock::now();
    is_running = false;
}

double Timer::elapsed() const {
    if (is_running) {
        std::cerr << "Timer is still running!" << std::endl;
        return 0.0;
    }
    return std::chrono::duration<double>(end_time - start_time).count();
}
