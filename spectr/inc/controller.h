/**
 * @file controller.h
 * @author Sergey Simonov (sb.simonov@gmail.com)
 * @brief Controller class header
 */
#pragma once
#include <string>
#include <memory>
#include <atomic>
#include <map>
#include "ocv.h"
#include "model.h"
#include "capture.h"

class MainWindow;
class SpectrView;
class VideoView;

/**
 *  Основной класс приложения (программный менеджер)
 *  Создает и управляет ресурсами прогарммы. Рализует рабочий цикл.
 */
class Controller {
public:
    enum class mode { video, spectr, roi_selct };
    
    Controller(const std::string& config_file);
    ~Controller();
    int run();
    void set_mode(mode m);
    void export_spectr();
    void spectr_memset();
    void spectr_memclear();
    void showgrid(int state);
    void set_spectr_fps(int fps);
    void set_rotation(int angle);
    void reset_roi();
    void calibrate(int n);
    void reset_calibration();
    Capture& get_capture();

    struct Options {
        int roi_x = 0, roi_y = 0, roi_width = 0, roi_height = 0;
        int gain = 5, exposure = 5;
        int spectr_acc_fps = 1;
        int calib_x[3] = {-1,-1,-1};
        int calib_v[3] = { 380, 522, 710};
        int rotation = 0;
        int showgrid = 0;

        void load(std::string filename);
        void save(std::string filename);
        cv::Rect roi();
        void set_roi(cv::Rect r);
        std::vector<std::pair<int, int>> calib_points();
    } opt;
    
    struct GlobalOptions {
        int spectr_win_width = 800;
        int spectr_win_height = 600;
        int calib_v[3] = { -1, -1, -1 };
        int gain_steps = 12;
        int gain_step_val = 5;
        int exposure_limit[2] = {-13, -1};
    } glob_opt;
    
private:
    volatile int rotation;
    std::atomic<mode> current_mode=mode::video;
    std::string config_file;
    std::string options_file;
    std::unique_ptr<Capture> capture = nullptr;
    std::unique_ptr<MainWindow> win_main;
    std::unique_ptr<Model_Spectr> model_spectr;
    std::unique_ptr<Model_Video> model_video;
    std::shared_ptr<SpectrView> view_spectr;
    std::shared_ptr<VideoView> view_video;
};




























