#include "dashboard.h"
#include <cmath>
#include <algorithm>

void Dashboard::draw(cv::Mat &frame, const TelemetryData &data)
{
    // 1. Создаем полупрозрачный фон для левой части экрана (панель 640x480)
    cv::Rect dashboard_rect(0, 0, 640, 480);
    if (frame.cols < 640 || frame.rows < 480)
    {
        dashboard_rect = cv::Rect(0, 0, frame.cols, frame.rows);
    }

    cv::Mat roi = frame(dashboard_rect);
    cv::Mat overlay;
    roi.copyTo(overlay);
    cv::rectangle(overlay, cv::Rect(0, 0, dashboard_rect.width, dashboard_rect.height), cv::Scalar(30, 30, 30), cv::FILLED);
    cv::addWeighted(overlay, 0.7, roi, 0.3, 0, roi); // Наложение с прозрачностью

    // 2. Отрисовка круглых приборов
    // Спидометр: 0-140 км/ч, красная зона с 90
    drawGauge(frame, cv::Point(160, 140), 100, data.speed_kmh, 140.0f, "Speed (km/h)", 90.0f);

    // Тахометр: 0-6000 RPM, красная зона с 4500
    drawGauge(frame, cv::Point(480, 140), 100, data.engine_rpm, 6000.0f, "RPM", 4500.0f);

    // 3. Отрисовка горизонтальных полос
    bool coolant_warn = data.coolant_temp > 100.0f;
    drawLinearGauge(frame, cv::Rect(40, 280, 560, 20), data.coolant_temp, 120.0f, "Coolant Temp (C)", coolant_warn);

    bool fuel_warn = data.fuel_level < 15.0f;
    drawLinearGauge(frame, cv::Rect(40, 330, 560, 20), data.fuel_level, 100.0f, "Fuel Level (%)", fuel_warn);

    drawLinearGauge(frame, cv::Rect(40, 380, 560, 20), data.throttle_pos, 100.0f, "Throttle (%)", false);

    // 4. Текст: Стиль вождения
    std::string style_text = "Style: ";
    cv::Scalar style_color;
    if (data.style_label == 0)
    {
        style_text += "SLOW";
        style_color = cv::Scalar(255, 255, 0); // Голубой (BGR)
    }
    else if (data.style_label == 1)
    {
        style_text += "NORMAL";
        style_color = cv::Scalar(0, 255, 0); // Зеленый
    }
    else
    {
        style_text += "AGGRESSIVE";
        style_color = cv::Scalar(0, 0, 255); // Красный
    }
    cv::putText(frame, style_text, cv::Point(40, 440), cv::FONT_HERSHEY_SIMPLEX, 0.8, style_color, 2);

    // 5. Предупреждения
    if (coolant_warn || fuel_warn)
    {
        std::string warn_msg = "WARNING: ";
        if (coolant_warn)
            warn_msg += "HIGH TEMP! ";
        if (fuel_warn)
            warn_msg += "LOW FUEL!";
        cv::putText(frame, warn_msg, cv::Point(280, 440), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 165, 255), 2); // Оранжевый
    }
}

void Dashboard::drawGauge(cv::Mat &frame, cv::Point center, int radius, float value, float max_value,
                          const std::string &title, float red_zone_threshold)
{
    int start_angle = 135;
    int end_angle = 405;
    int total_angle = end_angle - start_angle;

    float clamped_value = std::max(0.0f, std::min(value, max_value));

    // Фоновая дуга
    cv::ellipse(frame, center, cv::Size(radius, radius), 0, start_angle, end_angle, cv::Scalar(100, 100, 100), 4);

    // Заполненная дуга
    float value_ratio = clamped_value / max_value;
    int value_angle = start_angle + static_cast<int>(value_ratio * total_angle);
    cv::Scalar arc_color = (clamped_value >= red_zone_threshold) ? cv::Scalar(0, 0, 255) : cv::Scalar(0, 255, 0);
    cv::ellipse(frame, center, cv::Size(radius, radius), 0, start_angle, value_angle, arc_color, 4);

    // Стрелка
    double angle_rad = value_angle * CV_PI / 180.0;
    cv::Point needle_end(
        center.x + static_cast<int>(radius * 0.8 * cos(angle_rad)),
        center.y + static_cast<int>(radius * 0.8 * sin(angle_rad)));
    cv::line(frame, center, needle_end, cv::Scalar(255, 255, 255), 2);
    cv::circle(frame, center, 5, cv::Scalar(255, 255, 255), cv::FILLED);

    // Текст
    cv::putText(frame, title, cv::Point(center.x - 40, center.y + radius + 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(200, 200, 200), 1);

    char val_str[10];
    snprintf(val_str, sizeof(val_str), "%.1f", clamped_value);
    cv::putText(frame, val_str, cv::Point(center.x - 20, center.y + radius / 2), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
}

void Dashboard::drawLinearGauge(cv::Mat &frame, cv::Rect bounds, float value, float max_value,
                                const std::string &title, bool is_warning)
{
    float clamped_value = std::max(0.0f, std::min(value, max_value));
    float ratio = clamped_value / max_value;

    // Фон
    cv::rectangle(frame, bounds, cv::Scalar(80, 80, 80), 2);

    // Заполнение
    cv::Rect filled_rect = bounds;
    filled_rect.width = static_cast<int>(bounds.width * ratio);
    cv::Scalar fill_color = is_warning ? cv::Scalar(0, 0, 255) : cv::Scalar(255, 200, 0);
    cv::rectangle(frame, filled_rect, fill_color, cv::FILLED);

    // Текст
    cv::putText(frame, title, cv::Point(bounds.x, bounds.y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(200, 200, 200), 1);

    char val_str[10];
    snprintf(val_str, sizeof(val_str), "%.1f", clamped_value);
    cv::putText(frame, val_str, cv::Point(bounds.x + bounds.width - 40, bounds.y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
}