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
    cv::Mat frame;
private:
    Controller(const std::string& config_file);
    ~Controller();

    struct Options {
        int roi_x = 0, roi_y=0, roi_width=0, roi_height=0;
        int start_x = 380, accumulate_time = 1000;
        double delta_x = 0.3;
        void load(std::string filename);
        void save(std::string filename);
        cv::Rect roi();
        void set_roi(cv::Rect r);
    } opt;

    std::string config_file;
    std::string options_file;
    std::unique_ptr<Capture> capture = nullptr;
    std::unique_ptr<MainWindow> win_main;
    inline static std::atomic<mode> current_mode=mode::video;
    std::map<mode, std::unique_ptr<Model>> models;
    std::map<mode, std::shared_ptr<View>> views;
    

};








