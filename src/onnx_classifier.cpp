#include "onnx_classifier.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <iostream>

ONNXClassifier::ONNXClassifier(const std::string &model_path, const std::string &json_path)
    : env(ORT_LOGGING_LEVEL_WARNING, "ONNXClassifier")
{

    // 1. Загрузка параметров нормализации
    parseNormalizationParams(json_path);

    // 2. Настройка сессии
    Ort::SessionOptions session_options;
    session_options.SetIntraOpNumThreads(1);

#ifdef _WIN32
    std::wstring w_model_path(model_path.begin(), model_path.end());
    try
    {
        session = Ort::Session(env, w_model_path.c_str(), session_options);
    }
    catch (const Ort::Exception &e)
    {
        throw std::runtime_error(std::string("Failed to load ONNX model: ") + e.what());
    }
#else
    try
    {
        session = Ort::Session(env, model_path.c_str(), session_options);
    }
    catch (const Ort::Exception &e)
    {
        throw std::runtime_error(std::string("Failed to load ONNX model: ") + e.what());
    }
#endif
}

void ONNXClassifier::parseNormalizationParams(const std::string &json_path)
{
    std::ifstream file(json_path);
    if (!file.is_open())
    {
        throw std::runtime_error("Could not open JSON file: " + json_path);
    }

    // Читаем весь файл в строку
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    // Лямбда-функция для поиска массива чисел по ключу
    auto extract_array = [&content](const std::string &key) -> std::vector<float>
    {
        std::vector<float> result;
        size_t pos = content.find("\"" + key + "\"");
        if (pos == std::string::npos)
            throw std::runtime_error("Key not found in JSON: " + key);

        pos = content.find("[", pos);
        size_t end_pos = content.find("]", pos);
        if (pos == std::string::npos || end_pos == std::string::npos)
        {
            throw std::runtime_error("Invalid array format in JSON");
        }

        std::string array_str = content.substr(pos + 1, end_pos - pos - 1);
        std::stringstream ss(array_str);
        std::string item;

        while (std::getline(ss, item, ','))
        {
            result.push_back(std::stof(item));
        }
        return result;
    };

    mean_ = extract_array("mean");
    std_ = extract_array("std");

    if (mean_.size() != 6 || std_.size() != 6)
    {
        throw std::runtime_error("Normalization arrays must contain exactly 6 elements");
    }
}

ClassificationResult ONNXClassifier::classify(const std::vector<float> &features)
{
    if (features.size() != 6)
    {
        throw std::invalid_argument("Features must contain exactly 6 elements");
    }

    // 1. Нормализация (z-score)
    std::vector<float> normalized_features(6);
    for (size_t i = 0; i < 6; ++i)
    {
        normalized_features[i] = (features[i] - mean_[i]) / std_[i];
    }

    // 2. Подготовка входного тензора
    Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    std::vector<int64_t> input_shape = {1, 6}; // batch_size=1, features=6

    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
        memory_info, normalized_features.data(), normalized_features.size(),
        input_shape.data(), input_shape.size());

    const char *input_names[] = {"features"};
    const char *output_names[] = {"class_scores"};

    // 3. Запуск инференса
    auto output_tensors = session.Run(
        Ort::RunOptions{nullptr},
        input_names, &input_tensor, 1,
        output_names, 1);

    // 4. Применение Softmax
    float *floatarr = output_tensors.front().GetTensorMutableData<float>();
    std::vector<float> logits(floatarr, floatarr + 3);

    float max_logit = *std::max_element(logits.begin(), logits.end());
    float sum_exp = 0.0f;
    std::vector<float> probabilities(3);

    // Экспоненцирование (вычитаем max_logit для вычислительной стабильности)
    for (size_t i = 0; i < 3; ++i)
    {
        probabilities[i] = std::exp(logits[i] - max_logit);
        sum_exp += probabilities[i];
    }

    // Нормализация до 1.0 и поиск победителя
    float max_prob = 0.0f;
    int best_label = -1;
    for (size_t i = 0; i < 3; ++i)
    {
        probabilities[i] /= sum_exp;
        if (probabilities[i] > max_prob)
        {
            max_prob = probabilities[i];
            best_label = static_cast<int>(i);
        }
    }

    return ClassificationResult{best_label, max_prob, probabilities};
}