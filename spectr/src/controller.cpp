#include "controller.h"
#include "helpers.h"
#include <iostream>
#include "window.h"
#include "view.h"
#include "optlog.h"



Controller& Controller::instance(const std::string& config_file) {
    static Controller ctrl(config_file);
    return ctrl;
}


Controller::Controller(const std::string& config_file) {
    cv::FileStorage fs;
    if (!fs.open(config_file, cv::FileStorage::READ | cv::FileStorage::FORMAT_YAML)) {
        throw std::runtime_error(std::string("Can't open config file ") + config_file);
    }
    capture = Capture::create(fs);
    if (!capture || capture->width() == 0 || capture->height() == 0) {
        throw std::runtime_error("Bad capture device (width or height of frame is 0)");
    }
    win_main = std::make_unique<MainWindow>(this);

    // Live Video model and view
    models[mode::video] = std::make_unique<Model_Video>();
    views[mode::video] = std::make_shared<VideoView>(*win_main);
    models[mode::video]->subscribe(views[mode::video]);

    // Spectr model and view
    int start_x = load_or_default(fs, "spectr", "start_x", 380);
    double delta_x = load_or_default(fs, "spectr", "delta_x", static_cast<double>(std::abs(780 - start_x)) / capture->width());
    int accumulate_time_ms = load_or_default(fs, "spectr", "accumulate_time_ms", 1000);
    log1 << "start_x=" << start_x << std::endl;
    log1 << "delta_x=" << delta_x << std::endl;
    log1 << "accumulate_time_ms=" << accumulate_time_ms << std::endl;
    models[mode::spectr] = std::make_unique<Model_Spectr>(start_x, delta_x, accumulate_time_ms);
    views[mode::spectr] = std::make_shared<PlotView>(*win_main);
    models[mode::spectr]->subscribe(views[mode::spectr]);

    models[mode::roi_selct] = std::make_unique<Model_ROI_Select>();
}


int Controller::run() {
    
    while (win_main->visible()) {
        capture->read(frame);
      
        if (current_mode == mode::roi_selct) {
            win_main->overlayText("Задайте область интереса (ROI) с помощью мыши и нажмите пробел или Ввод", 5000);
            roi = cv::selectROI(static_cast<const char*>(*win_main), frame, true);
            log1 << "selected roi="<<roi<<std::endl;
            // for (auto& [k, v] : models) {
            //     v->set_ROI(rect);
            // }
            current_mode = mode::video;
        }
        if (roi == cv::Rect()) {
           models[current_mode]->udpate_data(frame); 
        }
        else {
            models[current_mode]->udpate_data(cv::Mat(frame, roi)); 
        }
        
        int key = cv::waitKey(5);
        if (key == ' ') {
            set_mode(current_mode == mode::spectr ? mode::video : mode::spectr);
        }
        else if (key == 'r') {
            set_mode(mode::roi_selct);
        };
        
    }
    return 0;
}


void Controller::set_mode(mode m) {
    log1<<"set__mode"<<(int)m<<std::endl;
    current_mode.store(m);
}

void Controller::set_roi() 
{
    roi = cv::selectROI(frame);
}

