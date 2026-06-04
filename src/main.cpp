#include <iostream>
#include <vector>
#include <iomanip>
#include "obd_parser.h"
#include "onnx_classifier.h"

int main()
{
    std::cout << "========================================" << std::endl;
    std::cout << "Real Car ADAS Monitor - Version 1.0" << std::endl;
    std::cout << "========================================\n"
              << std::endl;

    try
    {
        // 1. Загрузка данных
        OBDParser parser;
        int records_loaded = parser.load("../data/obd_data.csv"); // Путь из папки build
        if (records_loaded < 0)
        {
            std::cerr << "Error: Failed to load dataset." << std::endl;
            return -1;
        }

        // 2. Инициализация классификатора
        ONNXClassifier classifier("../models/driver_classifier.onnx",
                                  "../models/normalization_params.json");
        std::cout << "Classifier initialized successfully.\n"
                  << std::endl;

        // 3. Вывод таблицы для первых 20 записей
        std::cout << "--- Classification Results (First 20 records) ---" << std::endl;
        std::cout << std::left << std::setw(12) << "Record ID"
                  << std::setw(15) << "True Label"
                  << std::setw(15) << "Prediction"
                  << std::setw(15) << "Confidence" << std::endl;
        std::cout << std::string(55, '-') << std::endl;

        int correct_predictions = 0;
        int num_to_test = std::min(20, records_loaded);

        for (int i = 0; i < num_to_test; ++i)
        {
            auto record = parser.getRecord(i);

            std::vector<float> features = {
                record.speed_kmh,
                record.engine_rpm,
                record.throttle_pos,
                record.coolant_temp,
                record.fuel_level,
                record.intake_air_temp};

            ClassificationResult result = classifier.classify(features);

            if (result.label == record.label)
            {
                correct_predictions++;
            }

            std::cout << std::left << std::setw(12) << (i + 1)
                      << std::setw(15) << record.label
                      << std::setw(15) << result.label
                      << std::fixed << std::setprecision(2) << (result.confidence * 100.0f) << "%" << std::endl;
        }

        std::cout << std::string(55, '-') << std::endl;

        // 4. Подсчет точности
        float accuracy = (static_cast<float>(correct_predictions) / num_to_test) * 100.0f;
        std::cout << "Accuracy for first " << num_to_test << " records: " << accuracy << "%" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Critical Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}