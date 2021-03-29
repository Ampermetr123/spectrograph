#pragma once
#include <string>
#include <memory>
#include <map>
#include "ocv.h"

class Capture {
protected:
    cv::VideoCapture cap;
    static const std::map<const std::string, long> cap_props;
public:
    static std::unique_ptr<Capture> create(cv::FileStorage& fs);
    void set_property(std::string prop, double value);
    int height();
    int width();
    virtual void read(cv::Mat& frame) = 0;
    virtual ~Capture() = default;
    
};

class CameraCapture : public Capture {
public:
    CameraCapture(int devNo, cv::FileStorage& config);
    void read(cv::Mat& frame) override;
};


class FileCapture : public Capture {
    int delay_ms;
    const int64_t ticks_in_ms;
    int64_t grab_time = 0;
public:
    FileCapture(std::string path, cv::FileStorage& config);
    void read(cv::Mat& frame) override;
};








