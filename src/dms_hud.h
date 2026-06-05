/**
 * @file dms_hud.h
 * @brief Класс для отрисовки интерфейса DMS
 */
#ifndef DMS_HUD_H
#define DMS_HUD_H

#include <opencv2/opencv.hpp>
#include "dms_monitor.h"

/**
 * @class DMSHUD
 * @brief Отвечает за наложение графики DMS на кадр.
 */
class DMSHUD {
public:
    /**
     * @brief Отрисовывает интерфейс DMS.
     * @param main_frame Итоговый кадр 1280x480.
     * @param cam_frame Оригинальный кадр с веб-камеры.
     * @param state Состояние водителя.
     */
    void draw(cv::Mat& main_frame, const cv::Mat& cam_frame, const DriverState& state);

private:
    /**
     * @brief Рисует угловые скобки вокруг лица.
     * @param frame Кадр для отрисовки.
     * @param rect Координаты лица.
     * @param color Цвет рамок.
     */
    void drawCorners(cv::Mat& frame, const cv::Rect& rect, const cv::Scalar& color);
};

#endif // DMS_HUD_H