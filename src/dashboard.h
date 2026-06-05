/**
 * @file dashboard.h
 * @brief Модуль графической отрисовки приборной панели.
 */
#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <opencv2/opencv.hpp>
#include <string>

/**
 * @struct TelemetryData
 * @brief Данные для отображения на приборной панели.
 */
struct TelemetryData
{
    float speed_kmh;    /**< Скорость */
    float engine_rpm;   /**< Обороты двигателя */
    float coolant_temp; /**< Температура двигателя */
    float fuel_level;   /**< Уровень топлива */
    float throttle_pos; /**< Нажатие педали газа */
    int style_label;    /**< Текущий стиль вождения */
};

/**
 * @class Dashboard
 * @brief Класс для рендеринга виртуальной приборной панели автомобиля.
 */
class Dashboard
{
public:
    /**
     * @brief Отрисовывает приборную панель на заданном кадре.
     * @param frame Изображение (кадр), на котором будет нарисована панель.
     * @param data Структура с текущими показателями автомобиля.
     */
    void draw(cv::Mat &frame, const TelemetryData &data);

private:
    void drawGauge(cv::Mat &frame, cv::Point center, int radius, float min_val, float max_val, float current_val, const std::string &label, bool is_rpm);
    void drawLinearGauge(cv::Mat &frame, cv::Rect rect, float min_val, float max_val, float current_val, const std::string &label, cv::Scalar color, bool is_warning);
};

#endif // DASHBOARD_H