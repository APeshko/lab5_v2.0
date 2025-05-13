#pragma once
#include <vector>
#include "color.h"

// Генерация тестового изображения (градиент)
std::vector<std::vector<Color>> createTestImage(int height, int width);

// Сохранение изображения в PPM формате (очень простой формат)
bool savePPM(const std::vector<std::vector<Color>>& image, const std::string& filename);

// Загрузка изображения из PPM формата
std::vector<std::vector<Color>> loadPPM(const std::string& filename);
