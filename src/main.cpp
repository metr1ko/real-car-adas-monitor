/**
 * @file main.cpp
 * @brief Главный файл многопоточной ADAS системы.
 */
#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <opencv2/opencv.hpp>

#include "obd_parser.h"
#include "onnx_classifier.h"
#include "dashboard.h"
#include "dms_monitor.h"
#include "dms_hud.h"
#include "shared_state.h"

using namespace std::chrono;

/**
 * @brief Функция потока OBD. Считывает данные и классифицирует стиль вождения с частотой 10 Гц.
 * @param state Ссылка на общий ресурс (SharedState).
 * @param parser Ссылка на парсер телеметрии.
 * @param classifier Ссылка на ONNX классификатор.
 */
void obd_thread_func(SharedState &state, OBDParser &parser, ONNXClassifier &classifier)
{
    int total_records = parser.getRecordsCount();
    if (total_records == 0)
        return;

    int current_idx = 0;
    int last_label = -1;

    while (state.running)
    {
        if (current_idx >= total_records)
            current_idx = 0; // Зацикливаем датасет

        OBDRecord record = parser.getRecord(current_idx++);
        std::vector<float> features = {
            record.speed_kmh, record.engine_rpm, record.throttle_pos,
            record.coolant_temp, record.fuel_level, record.intake_air_temp};

        ClassificationResult res = classifier.classify(features);

        {
            std::lock_guard<std::mutex> lock(state.mtx);
            state.current_obd = record;
            state.current_style_label = res.label;
            state.total_obd_records++;

            if (res.label == 2 && last_label != 2)
            {
                state.aggressive_alerts++; // Считаем новые факты агрессии
            }
            last_label = res.label;
        }

        std::this_thread::sleep_for(milliseconds(100)); // 10 Hz
    }
}

int main()
{
    std::cout << "Starting Final ADAS System..." << std::endl;
    std::filesystem::create_directory("output");

    // 1. Инициализация модулей
    std::string base_path = "D:/study/mag/proging/real-car-adas-monitor/";

    OBDParser parser;
    parser.load(base_path + "data/obd_data.csv");

    ONNXClassifier classifier(base_path + "models/driver_classifier.onnx",
                              base_path + "models/normalization_params.json");

    DMSMonitor dms(base_path + "models/deploy.prototxt",
                   base_path + "models/res10_300x300_ssd_iter_140000.caffemodel",
                   base_path + "models/haarcascade_eye.xml");

    Dashboard dashboard;
    DMSHUD dms_hud;

    // 2. Инициализация логгера и видео
    std::string output_dir = base_path + "output/";
    std::filesystem::create_directory(output_dir);

    std::ofstream log_file(output_dir + "dms_alerts.log", std::ios::app);
    cv::VideoWriter video_writer(output_dir + "result_situation2.mp4", cv::VideoWriter::fourcc('m', 'p', '4', 'v'), 15.0, cv::Size(1280, 480));

    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        std::cerr << "Error: Camera not found." << std::endl;
        return -1;
    }

    SharedState shared_state;
    std::thread obd_thread(obd_thread_func, std::ref(shared_state), std::ref(parser), std::ref(classifier));

    cv::Mat cam_frame;
    cv::Mat main_frame = cv::Mat::zeros(480, 1280, CV_8UC3);

    bool was_drowsy = false;
    bool was_distracted = false;
    bool is_paused = false;

    auto start_time = steady_clock::now();

    while (shared_state.running)
    {
        if (!is_paused)
        {
            cap >> cam_frame;
            if (cam_frame.empty())
                break;

            DriverState dms_state = dms.analyze(cam_frame);
            main_frame.setTo(cv::Scalar(0, 0, 0));

            // Читаем потокобезопасные данные
            TelemetryData tdata;
            {
                std::lock_guard<std::mutex> lock(shared_state.mtx);
                tdata.speed_kmh = shared_state.current_obd.speed_kmh;
                tdata.engine_rpm = shared_state.current_obd.engine_rpm;
                tdata.coolant_temp = shared_state.current_obd.coolant_temp;
                tdata.fuel_level = shared_state.current_obd.fuel_level;
                tdata.throttle_pos = shared_state.current_obd.throttle_pos;
                tdata.style_label = shared_state.current_style_label;
            }

            // Отрисовка
            dashboard.draw(main_frame, tdata);
            dms_hud.draw(main_frame, cam_frame, dms_state);

            // Логирование алертов
            auto now = system_clock::to_time_t(system_clock::now());
            if (dms_state.alert_drowsy && !was_drowsy)
            {
                log_file << std::ctime(&now) << " - ALERT: DROWSINESS DETECTED" << std::endl;
                std::lock_guard<std::mutex> lock(shared_state.mtx);
                shared_state.drowsy_alerts++;
            }
            if (dms_state.alert_distracted && !was_distracted)
            {
                log_file << std::ctime(&now) << " - ALERT: DISTRACTION DETECTED" << std::endl;
                std::lock_guard<std::mutex> lock(shared_state.mtx);
                shared_state.distracted_alerts++;
            }
            was_drowsy = dms_state.alert_drowsy;
            was_distracted = dms_state.alert_distracted;

            video_writer.write(main_frame);
            cv::imshow("Real Car Monitor - ADAS", main_frame);
        }

        char key = (char)cv::waitKey(30);
        if (key == 'q' || key == 'Q' || key == 27)
        {
            shared_state.running = false;
        }
        else if (key == ' ')
        {
            is_paused = !is_paused;
        }
        else if (key == 's' || key == 'S')
        {
            std::string filename = output_dir + "screenshot_" + std::to_string(duration_cast<seconds>(steady_clock::now() - start_time).count()) + ".jpg";
            cv::imwrite(filename, main_frame);
            std::cout << "Screenshot saved to: " << filename << std::endl;
        }
    }

    obd_thread.join();
    auto end_time = steady_clock::now();
    double work_time = duration_cast<seconds>(end_time - start_time).count();

    // Шаг 7.5 Вывод итоговой статистики
    std::cout << "\n=== SYSTEM SHUTDOWN STATS ===" << std::endl;
    std::cout << "System Uptime: " << work_time << " seconds" << std::endl;
    std::cout << "OBD Records Processed: " << shared_state.total_obd_records << std::endl;
    std::cout << "Total Alerts: " << (shared_state.drowsy_alerts + shared_state.distracted_alerts + shared_state.aggressive_alerts) << std::endl;
    std::cout << " - Drowsiness: " << shared_state.drowsy_alerts << std::endl;
    std::cout << " - Distraction: " << shared_state.distracted_alerts << std::endl;
    std::cout << " - Aggressive Driving: " << shared_state.aggressive_alerts << std::endl;

    cap.release();
    video_writer.release();
    cv::destroyAllWindows();
    return 0;
}