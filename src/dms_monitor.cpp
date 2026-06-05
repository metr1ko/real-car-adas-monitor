#include "dms_monitor.h"

DMSMonitor::DMSMonitor(const std::string &face_prototxt, const std::string &face_caffe, const std::string &eye_cascade)
{
    face_net = cv::dnn::readNetFromCaffe(face_prototxt, face_caffe);
    if (face_net.empty())
    {
        throw std::runtime_error("Failed to load Face DNN model.");
    }

    if (!eye_classifier.load(eye_cascade))
    {
        throw std::runtime_error("Failed to load Eye Cascade model.");
    }
}

DriverState DMSMonitor::analyze(const cv::Mat &frame)
{
    DriverState state;

    // 1. Поиск лица
    state.face_rect = detectFace(frame);
    state.face_detected = (state.face_rect.area() > 0);

    if (state.face_detected)
    {
        // 2. Сначала проверяем поворот головы
        state.head_turn_deg = estimateHeadTurn(state.face_rect, frame.cols);
        state.looking_forward = (std::abs(state.head_turn_deg) < 20.0f); // Порог в 20 градусов

        // 3. Открытость глаз
        if (state.looking_forward)
        {
            state.eye_openness = estimateEyeOpenness(frame, state.face_rect);
            state.eyes_open = (state.eye_openness > 0.0f);
        }
        else
        {

            state.eyes_open = true;
        }
    }
    else
    {
        state.eyes_open = false;
        state.looking_forward = false;
    }

    // 4. Обновление истории глаз (15 кадров)
    eyes_history.push_back(state.eyes_open);
    if (eyes_history.size() > 15)
    {
        eyes_history.pop_front();
    }

    // 5. Логика алертов
    int closed_count = 0;
    for (bool open : eyes_history)
    {
        if (!open)
            closed_count++;
    }

    // Считаем отвлечение
    state.alert_distracted = !state.looking_forward;

    // Сонливость срабатывает только если нет отвлечения (жесткий приоритет)
    state.alert_drowsy = (closed_count >= 10) && !state.alert_distracted;

    return state;
}

cv::Rect DMSMonitor::detectFace(const cv::Mat &frame)
{
    cv::Mat blob = cv::dnn::blobFromImage(frame, 1.0, cv::Size(300, 300), cv::Scalar(104.0, 177.0, 123.0), false, false);
    face_net.setInput(blob);
    cv::Mat detection = face_net.forward();

    cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

    float max_confidence = 0.0f;
    cv::Rect best_face;

    for (int i = 0; i < detectionMat.rows; i++)
    {
        float confidence = detectionMat.at<float>(i, 2);
        if (confidence > 0.5f && confidence > max_confidence)
        { // Порог уверенности 0.5
            max_confidence = confidence;
            int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * frame.cols);
            int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * frame.rows);
            int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * frame.cols);
            int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * frame.rows);

            best_face = cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2));
        }
    }

    // Защита от выхода за границы
    if (best_face.area() > 0)
    {
        best_face &= cv::Rect(0, 0, frame.cols, frame.rows);
    }

    return best_face;
}

float DMSMonitor::estimateEyeOpenness(const cv::Mat &frame, const cv::Rect &face_rect)
{
    // Ищем глаза только в верхней половине лица
    cv::Rect top_half(face_rect.x, face_rect.y, face_rect.width, face_rect.height / 2);
    cv::Mat faceROI = frame(top_half);

    cv::Mat gray;
    cv::cvtColor(faceROI, gray, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(gray, gray);

    std::vector<cv::Rect> eyes;
    eye_classifier.detectMultiScale(gray, eyes, 1.1, 3, 0, cv::Size(20, 20));

    return (eyes.size() > 0) ? 1.0f : 0.0f;
}

float DMSMonitor::estimateHeadTurn(const cv::Rect &face_rect, int frame_width)
{
    float face_center_x = face_rect.x + face_rect.width / 2.0f;
    float frame_center_x = frame_width / 2.0f;

    // Смещение от центра (-1 до 1)
    float offset = (face_center_x - frame_center_x) / frame_center_x;

    return offset * 90.0f;
}