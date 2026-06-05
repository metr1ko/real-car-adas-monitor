#ifndef DMS_MONITOR_H
#define DMS_MONITOR_H

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <deque>
#include <string>

struct DriverState
{
    bool face_detected = false;
    bool eyes_open = true;
    bool looking_forward = true;
    float eye_openness = 1.0f;
    float head_turn_deg = 0.0f;
    bool alert_drowsy = false;
    bool alert_distracted = false;
    cv::Rect face_rect;
};

class DMSMonitor
{
public:
    // Конструктор загружает модели из файлов
    DMSMonitor(const std::string &face_prototxt, const std::string &face_caffe, const std::string &eye_cascade);

    // Главный метод анализа кадра
    DriverState analyze(const cv::Mat &frame);

private:
    cv::dnn::Net face_net;
    cv::CascadeClassifier eye_classifier;
    std::deque<bool> eyes_history; // Хранит последние 15 состояний глаз

    cv::Rect detectFace(const cv::Mat &frame);
    float estimateEyeOpenness(const cv::Mat &frame, const cv::Rect &face_rect);
    float estimateHeadTurn(const cv::Rect &face_rect, int frame_width);
};

#endif // DMS_MONITOR_H