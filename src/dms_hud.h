#ifndef DMS_HUD_H
#define DMS_HUD_H

#include <opencv2/opencv.hpp>
#include "dms_monitor.h"

class DMSHUD
{
public:
    void draw(cv::Mat &main_frame, const cv::Mat &cam_frame, const DriverState &state);

private:
    void drawCorners(cv::Mat &frame, const cv::Rect &rect, const cv::Scalar &color);
};

#endif // DMS_HUD_H