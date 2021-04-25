/**
 * @file window.h
 * @author Sergey Simonov (sb.simonov@gmail.com)
 *  Графические окна приложения
 */
#pragma once
#include <string>
#include "controller.h"

/** Базовый функционал для окна приложения  */
class Window {
protected:
    Window(const char* name);
    const char* WIN_NAME;
public:
    virtual ~Window() = default;
    virtual void draw(const cv::Mat& img);
    std::string name() const;
    bool visible();
};


/** Главное окно приложения */
class MainWindow : public Window {
    Controller*  ptr_ctrl;
public:
    MainWindow(Controller* controller);
    void create_controls();
    void overlayText(std::string text, int mstime);
};

