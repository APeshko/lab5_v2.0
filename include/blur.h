#pragma once
#include <vector>
#include "color.h"

class ImageBlur {
public:
    static std::vector<std::vector<Color>> sequentialBlur(const std::vector<std::vector<Color>>& image, int kernel_size = 3);
    static std::vector<std::vector<Color>> parallelBlurThreads(const std::vector<std::vector<Color>>& image, int kernel_size = 3, int num_threads = 4);
    
    #ifdef _OPENMP
    static std::vector<std::vector<Color>> parallelBlurOpenMP(const std::vector<std::vector<Color>>& image, int kernel_size = 3);
    #endif
};
