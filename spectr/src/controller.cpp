#include "controller.h"
#include <iostream>
#include "window.h"
#include "view.h"
#include "optlog.h"


Controller::Controller(){
   win_main = std::make_unique<MainWindow>(this);
}

Controller& Controller::instance() {
    static Controller ctrl;
    return ctrl;
}

int Controller::run() {
    log1<<"Started"<<std::endl;

    if (!make_video_capture()) {
        std::cerr << "ERROR! Unable to open camera\n";
        return 1;
    };
    on_start(); // setup models and views
    cv::Mat frame;
    while (win_main->visible() ){
        cap.read(frame);
        if (frame.empty()) {
            cap.set(cv::CAP_PROP_POS_FRAMES, 0);
            continue;
        }
        else {
         
            models[current_mode]->udpate_data(frame);
        }

        if (cv::waitKey(5) == ' ') {
            set_mode(current_mode == mode::spectr ? mode::video : mode::spectr);
        };
    }

    return 0;
}

void Controller::set_mode(mode m) 
{
    log1<<"set__mode"<<(int)m<<std::endl;
    current_mode.store(m);
}


void Controller::on_start() {
    using namespace cv;
    cv::Mat frame;
    cap.read(frame);
    std::cout << "FRAME info: channels " << frame.channels() << "; rows " << frame.rows << "; cols " << frame.cols << std::endl;
    std::stringstream ss;
    ss << "FPS: " << cap.get(CAP_PROP_FPS) << " W:" << cap.get(CAP_PROP_FRAME_WIDTH) << " H:" << cap.get(CAP_PROP_FRAME_HEIGHT)
        << " ISO:" << cap.get(CAP_PROP_ISO_SPEED) << " AUTO_EXP:" << cap.get(CAP_PROP_AUTO_EXPOSURE);
    std::cout << ss.str() << std::endl;

    models[mode::video] = std::make_unique<Model_Video>();
    views[mode::video] = std::make_shared<VideoView>(*win_main);
    models[mode::video]->subscribe(views[mode::video]);

    const double start_x = 380; //нм
    const double end_x = 780;
    const double dx = (end_x - start_x) / frame.cols;
    models[mode::spectr] = std::make_unique<Model_Spectr>(start_x, dx, 1000);
    views[mode::spectr] = std::make_shared<PlotView>(*win_main);
    models[mode::spectr]->subscribe(views[mode::spectr]);
}




bool Controller::make_video_capture() {
    int deviceID = 1;             // 0 = open default camera
    cap.open(deviceID, cv::CAP_DSHOW);
    if (!cap.isOpened()) {
        return false;
    }
    cap.set(cv::CAP_PROP_MONOCHROME, 1);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 1024);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 768);
    cap.set(cv::CAP_PROP_FPS, 10);
    return true;
}

bool Controller::make_file_capture() {
    cap.open("vid_test1/img_01.jpg", cv::CAP_IMAGES);
    if (!cap.isOpened()) {
        return false;
    }
    return true;
}

