#ifndef ONNX_CLASSIFIER_H
#define ONNX_CLASSIFIER_H

#include <string>
#include <vector>
#include <onnxruntime_cxx_api.h>

// Структура для возврата результатов инференса
struct ClassificationResult
{
    int label;                 // 0 (SLOW), 1 (NORMAL), 2 (AGGRESSIVE)
    float confidence;          // Уверенность (от 0.0 до 1.0)
    std::vector<float> scores; // Вероятности для каждого из 3 классов
};

class ONNXClassifier
{
public:
    // Загружает модель и параметры нормализации
    ONNXClassifier(const std::string &model_path, const std::string &json_path);

    // Метод классификации массива признаков
    ClassificationResult classify(const std::vector<float> &features);

private:
    Ort::Env env;
    Ort::Session session{nullptr};

    std::vector<float> mean_;
    std::vector<float> std_;

    // Внутренний метод для чтения JSON без сторонних библиотек
    void parseNormalizationParams(const std::string &json_path);
};

#endif // ONNX_CLASSIFIER_H