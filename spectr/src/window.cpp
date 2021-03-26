#include "window.h"



MainWindow::MainWindow(Controller* controller) : ptr_ctrl(controller) {
    cv::namedWindow(WIN_NAME, cv::WINDOW_AUTOSIZE | cv::WINDOW_GUI_EXPANDED);
    cv::setWindowTitle(WIN_NAME, "Спектрограф");

    cv::createButton("Видео", []([[maybe_unused]]int state, void* pctrl) {
            Controller* p= static_cast<Controller*>(pctrl);
            if (p){
                p->set_mode(Controller::mode::video);
            };
        }, &ptr_ctrl
    );

    cv::createButton("Спектр", []([[maybe_unused]]int state, void* pctrl) {
        Controller* p= static_cast<Controller*>(pctrl);
        if (p){
            p->set_mode(Controller::mode::spectr);
        };
    }, &ptr_ctrl
    );

    cv::startWindowThread();
}

void MainWindow::draw(cv::Mat& img) {
    cv::imshow(WIN_NAME, img);
}

void MainWindow::overlayText(std::string text, int mstime){
    cv::displayOverlay(WIN_NAME, text, mstime);
}

bool MainWindow::visible() 
{
    return cv::getWindowProperty(WIN_NAME, cv::WND_PROP_VISIBLE) > 0;
}


/*---------------------- NamedWindow  --------------------*/

NamedWindow::NamedWindow(std::string name):name(name) {   
    cv::namedWindow(name, cv::WINDOW_AUTOSIZE);
}

void NamedWindow::draw(cv::Mat& img) 
{
      cv::imshow(name, img);
}


bool NamedWindow::visible() 
{
    return cv::getWindowProperty(name, cv::WND_PROP_VISIBLE) > 0;
}
