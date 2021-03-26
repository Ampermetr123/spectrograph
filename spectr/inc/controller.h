#pragma once

#include "ocv.h"
#include "model.h"
#include <memory>
#include <atomic>
#include <map>

class MainWindow;
class View;

class Controller {
public:
    enum class mode{
        video,
        spectr
    };
    static Controller& instance();
    int run();
    void set_mode(mode m);
 
private:
    Controller();
    void on_start();
    bool make_video_capture();
    bool make_file_capture();

    cv::VideoCapture cap;
    std::unique_ptr<MainWindow> win_main;
    inline static std::atomic<mode> current_mode=mode::video;
    std::map<mode, std::unique_ptr<Model>> models;
    std::map<mode, std::shared_ptr<View>> views;
    std::mutex mtx;

};