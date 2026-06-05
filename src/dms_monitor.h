/**
 * @file dms_monitor.h
 * @brief Модуль мониторинга состояния водителя с использованием компьютерного зрения.
 */
#ifndef DMS_MONITOR_H
#define DMS_MONITOR_H

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <deque>
#include <string>

/**
 * @struct DriverState
 * @brief Структура, описывающая текущее состояние водителя.
 */
struct DriverState
{
    bool face_detected = false;    /**< Обнаружено ли лицо в кадре */
    bool eyes_open = true;         /**< Открыты ли глаза */
    bool looking_forward = true;   /**< Смотрит ли водитель прямо (на дорогу) */
    float eye_openness = 1.0f;     /**< Степень открытости глаз */
    float head_turn_deg = 0.0f;    /**< Угол поворота головы в градусах */
    bool alert_drowsy = false;     /**< Флаг предупреждения об усталости (сне) */
    bool alert_distracted = false; /**< Флаг предупреждения об отвлечении */
    cv::Rect face_rect;            /**< Координаты прямоугольника лица на кадре */
};

/**
 * @class DMSMonitor
 * @brief Класс для анализа кадра: находит лицо, оценивает направление взгляда и состояние глаз.
 */
class DMSMonitor
{
public:
    /**
     * @brief Конструктор загружает нейросетевые модели.
     * @param face_prototxt Путь к архитектуре сети (deploy.prototxt).
     * @param face_caffe Путь к весам сети (.caffemodel).
     * @param eye_cascade Путь к XML каскадам Хаара для поиска глаз.
     */
    DMSMonitor(const std::string &face_prototxt, const std::string &face_caffe, const std::string &eye_cascade);

    /**
     * @brief Главный метод анализа кадра.
     * @param frame Исходный кадр с веб-камеры (BGR).
     * @return Текущее состояние водителя (DriverState).
     */
    DriverState analyze(const cv::Mat &frame);

private:
    cv::dnn::Net face_net;                /**< Нейросеть для детекции лиц */
    cv::CascadeClassifier eye_classifier; /**< Каскад Хаара для детекции глаз */
    std::deque<bool> eyes_history;        /**< Хранит последние 15 состояний глаз */

    cv::Rect detectFace(const cv::Mat &frame);
    float estimateEyeOpenness(const cv::Mat &frame, const cv::Rect &face_rect);
    float estimateHeadTurn(const cv::Rect &face_rect, int frame_width);
};

#endif // DMS_MONITOR_H