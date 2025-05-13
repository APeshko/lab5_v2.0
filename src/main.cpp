#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include "blur.h"
#include "image_utils.h"
#include "timer.h"

void printHelp() {
    std::cout << "Usage:\n"
              << "  lab5_oop [options]\n\n"
              << "Options:\n"
              << "  --help              Show this help message\n"
              << "  --test <size>       Run tests with image of given size\n"
              << "  --input <file>      Input image file (PPM format)\n"
              << "  --output <file>     Output image file (PPM format)\n"
              << "  --threads <num>     Number of threads to use (default: 4)\n"
              << "  --kernel <size>     Blur kernel size (3 or 5, default: 3)\n";
}

void runTests(int image_size) {
    std::cout << "Running performance tests with image size " << image_size << "x" << image_size << "\n";
    
    // Создаем тестовое изображение
    auto image = createTestImage(image_size, image_size);
    
    Timer timer;
    
    // Тестируем последовательное размытие
    timer.start();
    auto seq_blur = ImageBlur::sequentialBlur(image);
    timer.stop();
    std::cout << "Sequential blur: " << timer.elapsed() << " seconds\n";
    
    // Тестируем параллельное размытие с разным количеством потоков
    for (int threads : {2, 4, 8}) {
        timer.start();
        auto par_blur = ImageBlur::parallelBlurThreads(image, 3, threads);
        timer.stop();
        std::cout << "Parallel blur (" << threads << " threads): " << timer.elapsed() << " seconds\n";
    }
    
    #ifdef _OPENMP
    // Тестируем OpenMP версию
    timer.start();
    auto omp_blur = ImageBlur::parallelBlurOpenMP(image);
    timer.stop();
    std::cout << "OpenMP blur: " << timer.elapsed() << " seconds\n";
    #endif
}

void processImage(const std::string& input_file, const std::string& output_file, int kernel_size, int num_threads) {
    try {
        // Загрузка изображения
        auto image = loadPPM(input_file);
        std::cout << "Image loaded: " << image.size() << "x" << image[0].size() << "\n";
        
        Timer timer;
        
        // Обработка изображения
        timer.start();
        auto blurred_image = ImageBlur::parallelBlurThreads(image, kernel_size, num_threads);
        timer.stop();
        
        std::cout << "Image processed in " << timer.elapsed() << " seconds\n";
        
        // Сохранение результата
        if (savePPM(blurred_image, output_file)) {
            std::cout << "Result saved to " << output_file << "\n";
        } else {
            std::cerr << "Failed to save result image\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

void atomic_example() {
    const int iterations = 1000000;
    const int thread_count = 10;
    
    std::cout << "\nAtomic operations example (" << thread_count << " threads, " 
              << iterations << " iterations each):\n";
    
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
        
        std::cout << "Mutex version: " << timer.elapsed() << " seconds, counter = " << counter << "\n";
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
        
        std::cout << "Atomic version: " << timer.elapsed() << " seconds, counter = " << counter << "\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        // Режим по умолчанию - демонстрация работы
        std::cout << "Running in demonstration mode\n";
        runTests(1000);
        atomic_example();
        return 0;
    }
    
    // Обработка аргументов командной строки
    std::string input_file, output_file;
    int kernel_size = 3;
    int num_threads = 4;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printHelp();
            return 0;
        } else if (arg == "--test" && i + 1 < argc) {
            int size = std::stoi(argv[++i]);
            runTests(size);
            return 0;
        } else if (arg == "--input" && i + 1 < argc) {
            input_file = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            output_file = argv[++i];
        } else if (arg == "--threads" && i + 1 < argc) {
            num_threads = std::stoi(argv[++i]);
        } else if (arg == "--kernel" && i + 1 < argc) {
            kernel_size = std::stoi(argv[++i]);
            if (kernel_size != 3 && kernel_size != 5) {
                std::cerr << "Kernel size must be 3 or 5\n";
                return 1;
            }
        }
    }
    
    if (!input_file.empty() && !output_file.empty()) {
        processImage(input_file, output_file, kernel_size, num_threads);
    } else {
        printHelp();
        return 1;
    }
    
    return 0;
}
