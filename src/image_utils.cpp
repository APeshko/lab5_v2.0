#include "image_utils.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <random>

std::vector<std::vector<Color>> createTestImage(int height, int width) {
    std::vector<std::vector<Color>> image(height, std::vector<Color>(width));
    
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            // Создаем градиентное изображение
            image[i][j].r = static_cast<unsigned char>(255 * i / height);
            image[i][j].g = static_cast<unsigned char>(255 * j / width);
            image[i][j].b = static_cast<unsigned char>(255 * (i + j) / (height + width));
        }
    }
    
    return image;
}

bool savePPM(const std::vector<std::vector<Color>>& image, const std::string& filename) {
    if (image.empty() || image[0].empty()) return false;
    
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    
    int height = image.size();
    int width = image[0].size();
    
    // Записываем заголовок PPM файла
    file << "P6\n" << width << " " << height << "\n255\n";
    
    // Записываем данные пикселей
    for (const auto& row : image) {
        for (const auto& pixel : row) {
            file.write(reinterpret_cast<const char*>(&pixel), 3);
        }
    }
    
    file.close();
    return true;
}

std::vector<std::vector<Color>> loadPPM(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    std::string format;
    int width, height, max_val;
    
    file >> format;
    if (format != "P6") {
        throw std::runtime_error("Unsupported PPM format: " + format);
    }
    
    file >> width >> height >> max_val;
    file.ignore(1); // Пропускаем один символ (обычно это перевод строки)
    
    std::vector<std::vector<Color>> image(height, std::vector<Color>(width));
    
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            file.read(reinterpret_cast<char*>(&image[i][j]), 3);
        }
    }
    
    file.close();
    return image;
}
