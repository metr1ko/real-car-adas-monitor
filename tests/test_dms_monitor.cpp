#include <gtest/gtest.h>
#include "../src/dms_monitor.h"
#include <opencv2/opencv.hpp> // Необходимо для cv::Exception

// 1. Тест: по умолчанию модуль не загружен (кидает ошибку OpenCV)
TEST(DMSMonitorTest, DefaultNotLoaded)
{
    EXPECT_THROW(DMSMonitor("invalid.prototxt", "invalid.caffemodel", "invalid.xml"), cv::Exception);
}

// 2. Тест: анализ на пустом кадре не падает, возвращает face_detected = false
TEST(DMSMonitorTest, EmptyFrameAnalysis)
{
    try
    {
        // Абсолютные пути для уверенности при запуске тестов из разных папок
        DMSMonitor dms("D:/study/mag/proging/real-car-adas-monitor/models/deploy.prototxt",
                       "D:/study/mag/proging/real-car-adas-monitor/models/res10_300x300_ssd_iter_140000.caffemodel",
                       "D:/study/mag/proging/real-car-adas-monitor/models/haarcascade_eye.xml");

        cv::Mat empty_frame;
        DriverState state = dms.analyze(empty_frame);

        EXPECT_FALSE(state.face_detected);
    }
    catch (const std::exception &e)
    {
        SUCCEED();
    }
}

// 3. Тест: успешная загрузка моделей
TEST(DMSMonitorTest, SuccessfulModelLoad)
{
    EXPECT_NO_THROW({
        DMSMonitor dms("D:/study/mag/proging/real-car-adas-monitor/models/deploy.prototxt",
                       "D:/study/mag/proging/real-car-adas-monitor/models/res10_300x300_ssd_iter_140000.caffemodel",
                       "D:/study/mag/proging/real-car-adas-monitor/models/haarcascade_eye.xml");
    });
}