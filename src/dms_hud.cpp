#include "dms_hud.h"

void DMSHUD::draw(cv::Mat &main_frame, const cv::Mat &cam_frame, const DriverState &state)
{
    // 1. Размещаем камеру в правой части экрана по центру вертикали (640x480)
    cv::Mat resized_cam;
    cv::resize(cam_frame, resized_cam, cv::Size(640, 480));

    cv::Rect target_rect(640, (720 - 480) / 2, 640, 480);
    resized_cam.copyTo(main_frame(target_rect));

    cv::Mat roi = main_frame(target_rect);

    // 2. Рисуем угловые рамки лица
    if (state.face_detected)
    {
        // Пересчитываем координаты лица с учетом масштабирования и смещения
        float scale_x = 640.0f / cam_frame.cols;
        float scale_y = 480.0f / cam_frame.rows;

        cv::Rect scaled_face(
            static_cast<int>(state.face_rect.x * scale_x),
            static_cast<int>(state.face_rect.y * scale_y),
            static_cast<int>(state.face_rect.width * scale_x),
            static_cast<int>(state.face_rect.height * scale_y));

        cv::Scalar color = state.alert_drowsy ? cv::Scalar(0, 165, 255) : cv::Scalar(0, 255, 0); // Оранжевый или зеленый
        drawCorners(roi, scaled_face, color);
    }

    // 3. Текстовые индикаторы сверху справа (внутри ROI)
    cv::putText(roi, "DMS STATUS", cv::Point(20, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);

    cv::Scalar eye_color = state.eyes_open ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255);
    cv::putText(roi, "EYES: " + std::string(state.eyes_open ? "OPEN" : "CLOSED"), cv::Point(20, 60), cv::FONT_HERSHEY_SIMPLEX, 0.6, eye_color, 2);

    cv::Scalar head_color = state.looking_forward ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255);
    cv::putText(roi, "HEAD: " + std::string(state.looking_forward ? "FORWARD" : "TURNED"), cv::Point(20, 90), cv::FONT_HERSHEY_SIMPLEX, 0.6, head_color, 2);

    // 4. Алерты
    if (state.alert_drowsy)
    {
        // Плашка по центру
        cv::Rect banner(0, 200, 640, 80);
        cv::Mat overlay;
        roi.copyTo(overlay);
        cv::rectangle(overlay, banner, cv::Scalar(0, 165, 255), cv::FILLED); // Оранжевый фон
        cv::addWeighted(overlay, 0.6, roi, 0.4, 0, roi);
        cv::putText(roi, "DROWSINESS ALERT!", cv::Point(140, 250), cv::FONT_HERSHEY_SIMPLEX, 1.2, cv::Scalar(255, 255, 255), 3);
    }

    if (state.alert_distracted)
    {
        // Красная полоса снизу
        cv::rectangle(roi, cv::Rect(0, 460, 640, 20), cv::Scalar(0, 0, 255), cv::FILLED);
        cv::putText(roi, "DISTRACTION", cv::Point(240, 475), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
    }
}

void DMSHUD::drawCorners(cv::Mat &frame, const cv::Rect &rect, const cv::Scalar &color)
{
    int length = 20;
    int thickness = 3;
    int x = rect.x, y = rect.y, w = rect.width, h = rect.height;

    // Левый верхний
    cv::line(frame, cv::Point(x, y), cv::Point(x + length, y), color, thickness);
    cv::line(frame, cv::Point(x, y), cv::Point(x, y + length), color, thickness);
    // Правый верхний
    cv::line(frame, cv::Point(x + w, y), cv::Point(x + w - length, y), color, thickness);
    cv::line(frame, cv::Point(x + w, y), cv::Point(x + w, y + length), color, thickness);
    // Левый нижний
    cv::line(frame, cv::Point(x, y + h), cv::Point(x + length, y + h), color, thickness);
    cv::line(frame, cv::Point(x, y + h), cv::Point(x, y + h - length), color, thickness);
    // Правый нижний
    cv::line(frame, cv::Point(x + w, y + h), cv::Point(x + w - length, y + h), color, thickness);
    cv::line(frame, cv::Point(x + w, y + h), cv::Point(x + w, y + h - length), color, thickness);
}