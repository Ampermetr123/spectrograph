/**
 * @file view.h
 * @author Sergey Simonov (sb.simonov@gmail.com)
 * Классы представления (видов) - отобржают данные модели
 */

#pragma once
#include <functional>
#include "ocv.h"
#include "model.h"
#include "window.h"
#include "fps.h"


/** Базовый класс вида */
class View : public ModelSubscriber {
protected:
    Window& window;
    bool is_active = false;
public:
    View(Window& w) : window(w) {};
    virtual void activate() { is_active = true; };
    virtual void deactivate() { is_active = false; };
};


/** Отображение видеокадров */
class VideoView : public View {
public:
    VideoView(Window& w);
    void showgrid(bool state);
    void on_data_updated(Model& m) override;
private:
    FPS_Stat fps;
    bool add_grid;
};


/** Отображение спектров в виде графика */
class SpectrView : public View {
public:
    SpectrView(Window& w, int width = 800, int height = 600);
    void activate() override;
    void pick_X(std::function<void(int x)> callback);
    void on_data_updated(Model& m) override;
private:
    void setup_mouse_handler();
    static void mouse_handler(int event, int x, int y, int flags, void* userdata);
    std::pair<double, double> calc_limit(const cv::Mat &yData);
    const int width, height; // размер вида
    const int Y_LIM_STEP = 50; // дисрктная величина для увеличения шкалы Y
    CvPlot::Axes axes;
    bool pick_X_mode = false;
    std::function<void(int x)> pick_X_callback;
    FPS_Stat fps;
    int mouse_pos_x = -1;
    int mouse_pos_y = -1;
    std::pair<double, double> mem_spectr_y_limits;
};
