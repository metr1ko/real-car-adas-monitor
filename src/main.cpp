#include <iostream>
#include <opencv2/opencv.hpp>
#include "dashboard.h"

int main()
{
    std::cout << "Starting Dashboard Test..." << std::endl;

    // Создаем пустой кадр (имитация видео с камеры 1280x720)
    cv::Mat frame = cv::Mat::zeros(720, 1280, CV_8UC3);

    // Немного "шума" на фон для реалистичности
    cv::randu(frame, cv::Scalar(0, 0, 0), cv::Scalar(50, 50, 50));

    Dashboard dashboard;

    // Тестовые данные (создаем предупреждение по топливу и стилю)
    TelemetryData test_data;
    test_data.speed_kmh = 105.5f;   // Спидометр: красная зона
    test_data.engine_rpm = 3200.0f; // Тахометр: зеленая зона
    test_data.coolant_temp = 90.0f; // Температура: норма
    test_data.fuel_level = 10.0f;   // Топливо: меньше 15% - триггер Warning!
    test_data.throttle_pos = 45.0f; // Дроссель: норма
    test_data.style_label = 2;      // Стиль: AGGRESSIVE

    // Отрисовываем панель поверх кадра
    dashboard.draw(frame, test_data);

    // Вывод на экран
    cv::imshow("Real Car Monitor - Dashboard Test", frame);
    std::cout << "Press any key on the image window to close..." << std::endl;
    cv::waitKey(0);

    return 0;
}