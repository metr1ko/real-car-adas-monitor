/**
 * @file onnx_classifier.h
 * @brief Модуль ИИ для классификации стиля вождения с использованием ONNX Runtime.
 */
#ifndef ONNX_CLASSIFIER_H
#define ONNX_CLASSIFIER_H

#include <string>
#include <vector>
#include <onnxruntime_cxx_api.h>

/**
 * @struct ClassificationResult
 * @brief Результат работы нейросети.
 */
struct ClassificationResult
{
    int label;        /**< Метка класса: 0=SLOW, 1=NORMAL, 2=AGGRESSIVE */
    float confidence; /**< Уверенность сети в предсказании (0.0 - 1.0) */
};

/**
 * @class ONNXClassifier
 * @brief Класс для загрузки ONNX модели и выполнения инференса.
 */
class ONNXClassifier
{
public:
    /**
     * @brief Конструктор инициализирует модель и параметры нормализации.
     * @param model_path Путь к файлу .onnx.
     * @param norm_params_path Путь к JSON файлу с параметрами нормализации.
     */
    ONNXClassifier(const std::string &model_path, const std::string &norm_params_path);

    /**
     * @brief Классифицирует текущие данные телеметрии.
     * @param features Вектор признаков (скорость, обороты и т.д.).
     * @return Результат классификации (класс и уверенность).
     */
    ClassificationResult classify(const std::vector<float> &features);

private:
    Ort::Env env;
    Ort::Session session{nullptr};
    std::vector<float> means;
    std::vector<float> scales;
};

#endif // ONNX_CLASSIFIER_H