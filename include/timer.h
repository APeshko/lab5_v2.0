#pragma once
#include <chrono>

class Timer {
public:
    void start();
    void stop();
    double elapsed() const;
    
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    std::chrono::time_point<std::chrono::high_resolution_clock> end_time;
    bool is_running = false;
};
