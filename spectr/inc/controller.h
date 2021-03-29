#pragma once

#include "ocv.h"
#include "model.h"
#include "capture.h"

#include <string>
#include <memory>
#include <atomic>
#include <map>

class MainWindow;
class View;

class Controller {
public:
    enum class mode{
        video,
        spectr,
        roi_selct
    };
    static Controller& instance(const std::string& config_file);
    int run();
    void set_mode(mode m);
    void set_roi();
    cv::Mat frame;
private:
    Controller(const std::string& config_file);
    
    std::unique_ptr<Capture> capture = nullptr;
    std::unique_ptr<MainWindow> win_main;
    inline static std::atomic<mode> current_mode=mode::video;
    std::map<mode, std::unique_ptr<Model>> models;
    std::map<mode, std::shared_ptr<View>> views;
   
    cv::Rect roi;

};

