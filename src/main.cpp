#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include "blur.h"
#include "image_utils.h"
#include "timer.h"

void atomic_example() {
    const int iterations = 1000000;
    const int thread_count = 10;
    
    // Версия с мьютексом
    {
        int counter = 0;
        std::mutex mtx;
        std::vector<std::thread> threads;
        Timer timer;
        
        timer.start();
        for (int i = 0; i < thread_count; ++i) {
            threads.emplace_back([&]() {
                for (int j = 0; j < iterations; ++j) {
                    std::lock_guard<std::mutex> lock(mtx);
                    counter++;
                }
            });
        }
        
        for (auto& t : threads) {
            t.join();
        }
        timer.stop();
        
        std::cout << "Mutex version: " << timer.elapsed() << " seconds, counter = " << counter << std::endl;
    }
    
    // Версия с atomic
    {
        std::atomic<int> counter(0);
        std::vector<std::thread> threads;
        Timer timer;
        
        timer.start();
        for (int i = 0; i < thread_count; ++i) {
            threads.emplace_back([&]() {
                for (int j = 0; j < iterations; ++j) {
                    counter++;
                }
            });
        }
        
        for (auto& t : threads) {
            t.join();
        }
        timer.stop();
        
        std::cout << "Atomic version: " << timer.elapsed() << " seconds, counter = " << counter << std::endl;
    }
}

int main() {
    // Создаем тестовое изображение
    const int width = 1000;
    const int height = 1000;
    auto image = createTestImage(height, width);
    
    // Последовательное размытие
    Timer timer;
    timer.start();
    auto seq_blur = ImageBlur::sequentialBlur(image);
    timer.stop();
    std::cout << "Sequential blur: " << timer.elapsed() << " seconds" << std::endl;
    
    // Параллельное размытие с потоками
    timer.start();
    auto par_blur = ImageBlur::parallelBlurThreads(image, 3, 4);
    timer.stop();
    std::cout << "Parallel blur (threads): " << timer.elapsed() << " seconds" << std::endl;
    
    #ifdef _OPENMP
    // Параллельное размытие с OpenMP
    timer.start();
    auto omp_blur = ImageBlur::parallelBlurOpenMP(image);
    timer.stop();
    std::cout << "Parallel blur (OpenMP): " << timer.elapsed() << " seconds" << std::endl;
    #endif
    
    // Пример с атомарными операциями
    atomic_example();
    
    return 0;
}
