#pragma once

#include <string>
#include <memory>
#include "ocv.h"
#include "controller.h"

class Window {
public:
    virtual void draw(cv::Mat& img) = 0;
};


class MainWindow : public Window {
    const char* WIN_NAME = "SpectrMainWindow";    
    Controller*  ptr_ctrl;
public:
    MainWindow(Controller* controller);
    operator const char* () {
        return WIN_NAME;
    } 
    //MainWindow(const MainWindow&) = delete;
  //  MainWindow(MainWindow&&) = delete;
    void draw(cv::Mat& img) override;
    void overlayText(std::string text, int mstime);
    bool visible();
};



class NamedWindow : public Window {
    std::string name;
public:
    NamedWindow(std::string name);
    void draw(cv::Mat& img) override;
    bool visible();
};