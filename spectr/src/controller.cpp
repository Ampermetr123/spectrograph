
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


Controller::Controller(const std::string& config_file) : config_file(config_file) {

    options_file = get_user_dir() + std::string("\\spectr.options.yml");
    log1 << "Options file" << options_file;

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
    opt.load(options_file);
    
    
    int accumulate_time_ms = load_or_default(fs, "spectr", "accumulate_time_ms", 1000);
    log1 << "start_x=" << opt.start_x << std::endl;
    log1 << "delta_x=" << opt.delta_x << std::endl;
    log1 << "accumulate_time_ms=" << opt.accumulate_time << std::endl;
    models[mode::spectr] = std::make_unique<Model_Spectr>(opt.start_x, opt.delta_x, opt.accumulate_time);
    views[mode::spectr] = std::make_shared<PlotView>(*win_main);
    models[mode::spectr]->subscribe(views[mode::spectr]);
}

Controller::~Controller() 
{
    opt.save(options_file);
}

int Controller::run() {
    
    while (win_main->visible()) {
        capture->read(frame);
        
        if (current_mode == mode::roi_selct) {
            win_main->overlayText("Задайте область интереса (ROI) с помощью мыши и нажмите пробел или Ввод", 5000);
            opt.set_roi(cv::selectROI(static_cast<const char*>(*win_main), frame, true));
            log1 << "selected roi=" << opt.roi() << std::endl;
            current_mode = mode::video;
            continue;
        }

        auto roi = opt.roi();
        
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




void Controller::Options::load(std::string file_path) {
    cv::FileStorage fs(file_path, cv::FileStorage::READ);
    start_x = load_or_default(fs, "X_SCALE", "start_x", start_x);
    delta_x = load_or_default(fs, "X_SCALE", "delta_x", delta_x);
    roi_x = load_or_default(fs, "ROI", "x", roi_x);
    roi_y = load_or_default(fs, "ROI", "y", roi_y);
    roi_width = load_or_default(fs, "ROI", "width", roi_width);
    roi_height = load_or_default(fs, "ROI", "height", roi_height);
    accumulate_time = load_or_default(fs, "" , "accumulate_time", accumulate_time);
    
}

void Controller::Options::save(std::string file_path) {
    cv::FileStorage fs(file_path, cv::FileStorage::WRITE);    
    fs.startWriteStruct("X_SCALE",cv::FileNode::MAP);
        fs.write("start_x",start_x);
        fs.write("delta_x",delta_x);
    fs.endWriteStruct();
    fs.startWriteStruct("ROI",cv::FileNode::MAP);
        fs.write("x",roi_x);
        fs.write("y",roi_y);
        fs.write("width",roi_width);
        fs.write("height",roi_height);
    fs.endWriteStruct();
    fs.write("accumulate_time", accumulate_time);
}

cv::Rect Controller::Options::roi() 
{
    return cv::Rect(roi_x,roi_y,roi_width,roi_height);
}

void Controller::Options::set_roi(cv::Rect r) 
{
    roi_x = r.x;
    roi_y = r.y;
    roi_width = r.width;
    roi_height = r.height;
}