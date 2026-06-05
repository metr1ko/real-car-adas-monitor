#include <iostream>
#include <opencv2/opencv.hpp>
#include "dms_monitor.h"
#include "dms_hud.h"

int main()
{
    std::cout << "Starting DMS Test..." << std::endl;

    // Пути к моделям
    std::string face_proto = "D:/study/mag/proging/real-car-adas-monitor/models/deploy.prototxt";
    std::string face_model = "D:/study/mag/proging/real-car-adas-monitor/models/res10_300x300_ssd_iter_140000.caffemodel";
    std::string eye_cascade = "D:/study/mag/proging/real-car-adas-monitor/models/haarcascade_eye.xml";

    DMSMonitor dms(face_proto, face_model, eye_cascade);
    DMSHUD hud;

    // Открываем веб-камеру
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open web camera!" << std::endl;
        return -1;
    }

    cv::Mat cam_frame;
    cv::Mat main_frame = cv::Mat::zeros(720, 1280, CV_8UC3); // Главный черный фон 1280x720

    std::cout << "Press 'q' or ESC to exit." << std::endl;

    while (true)
    {
        cap >> cam_frame;
        if (cam_frame.empty())
            break;

        // 1. Анализируем кадр
        DriverState state = dms.analyze(cam_frame);

        // 2. Очищаем главный фон (оставляем левую половину черной)
        main_frame.setTo(cv::Scalar(0, 0, 0));

        // 3. Рисуем интерфейс DMS на правой половине
        hud.draw(main_frame, cam_frame, state);

        // 4. Показываем результат
        cv::imshow("Real Car Monitor - DMS Test", main_frame);

        char key = (char)cv::waitKey(30);
        if (key == 27 || key == 'q' || key == 'Q')
            break; // ESC или 'q' для выхода
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}