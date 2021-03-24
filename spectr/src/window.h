#pragma once
#include "ocv.h"
#include <string>

class Window {
public:
    virtual void draw(cv::Mat& img) = 0;
};




class MainWindow : public Window {
    const char* WIN_NAME = "SpectrMainWindow";    
    MainWindow();

public:
    static MainWindow& instance();
    operator const char* () {
        return WIN_NAME;
    } 
    MainWindow(const MainWindow&) = delete;
    MainWindow(MainWindow&&) = delete;
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