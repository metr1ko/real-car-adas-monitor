#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <opencv2/opencv.hpp>
#include <string>

// Структура для передачи текущего кадра телеметрии
struct TelemetryData
{
    float speed_kmh;
    float engine_rpm;
    float throttle_pos;
    float coolant_temp;
    float fuel_level;
    int style_label; // 0=SLOW, 1=NORMAL, 2=AGGRESSIVE
};

class Dashboard
{
public:
    // Главный метод отрисовки (собирает всю панель)
    void draw(cv::Mat &frame, const TelemetryData &data);

private:
    // Отрисовка круглого прибора (спидометр, тахометр)
    void drawGauge(cv::Mat &frame, cv::Point center, int radius, float value, float max_value,
                   const std::string &title, float red_zone_threshold);

    // Отрисовка горизонтальной полосы (температура, топливо, дроссель)
    void drawLinearGauge(cv::Mat &frame, cv::Rect bounds, float value, float max_value,
                         const std::string &title, bool is_warning);
};

#endif // DASHBOARD_H