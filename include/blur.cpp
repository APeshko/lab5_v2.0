#include "blur.h"
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>

namespace {
    std::mutex mutex;
    
    Color calculateBlurredPixel(const std::vector<std::vector<Color>>& image, int x, int y, int kernel_size) {
        int r = 0, g = 0, b = 0;
        int count = 0;
        
        int half_kernel = kernel_size / 2;
        int height = image.size();
        int width = image[0].size();
        
        for (int i = -half_kernel; i <= half_kernel; ++i) {
            for (int j = -half_kernel; j <= half_kernel; ++j) {
                int nx = x + i;
                int ny = y + j;
                
                if (nx >= 0 && nx < height && ny >= 0 && ny < width) {
                    r += image[nx][ny].r;
                    g += image[nx][ny].g;
                    b += image[nx][ny].b;
                    count++;
                }
            }
        }
        
        if (count == 0) return image[x][y];
        
        return Color(static_cast<unsigned char>(r / count),
                     static_cast<unsigned char>(g / count),
                     static_cast<unsigned char>(b / count));
    }
    
    void processImagePart(const std::vector<std::vector<Color>>& image, 
                         std::vector<std::vector<Color>>& result,
                         int start_row, int end_row, 
                         int kernel_size) {
        for (int i = start_row; i < end_row; ++i) {
            for (size_t j = 0; j < image[i].size(); ++j) {
                result[i][j] = calculateBlurredPixel(image, i, j, kernel_size);
            }
        }
    }
}

std::vector<std::vector<Color>> ImageBlur::sequentialBlur(const std::vector<std::vector<Color>>& image, int kernel_size) {
    std::vector<std::vector<Color>> result(image.size(), std::vector<Color>(image[0].size()));
    processImagePart(image, result, 0, image.size(), kernel_size);
    return result;
}

std::vector<std::vector<Color>> ImageBlur::parallelBlurThreads(const std::vector<std::vector<Color>>& image, int kernel_size, int num_threads) {
    std::vector<std::vector<Color>> result(image.size(), std::vector<Color>(image[0].size()));
    std::vector<std::thread> threads;
    
    int rows_per_thread = image.size() / num_threads;
    
    for (int i = 0; i < num_threads; ++i) {
        int start_row = i * rows_per_thread;
        int end_row = (i == num_threads - 1) ? image.size() : (i + 1) * rows_per_thread;
        
        threads.emplace_back(processImagePart, std::cref(image), std::ref(result), 
                           start_row, end_row, kernel_size);
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    return result;
}

#ifdef _OPENMP
std::vector<std::vector<Color>> ImageBlur::parallelBlurOpenMP(const std::vector<std::vector<Color>>& image, int kernel_size) {
    std::vector<std::vector<Color>> result(image.size(), std::vector<Color>(image[0].size()));
    
    #pragma omp parallel for
    for (int i = 0; i < image.size(); ++i) {
        for (size_t j = 0; j < image[i].size(); ++j) {
            result[i][j] = calculateBlurredPixel(image, i, j, kernel_size);
        }
    }
    
    return result;
}
#endif
