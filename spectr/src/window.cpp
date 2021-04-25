/**
 * @file window.cpp
 * @author Sergey Simonov (sb.simonov@gmail.com)
 */
#ifdef _MSC_VER
#pragma warning(default: 4100 5054) 
#endif

#include "window.h"
#include "version.h"

/*-------------------------  Window Base --------------------------------*/

Window::Window(const char* name) : WIN_NAME(name) {};

/** Возвращает true, если окно существует (не закрыто) */
bool Window::visible() {
    return cv::getWindowProperty(WIN_NAME, cv::WND_PROP_VISIBLE) > 0;
}

/** Возвращает имя окна, котором можно использовать в OpenCv функциях работы с окнами */
std::string Window::name() const {
    return std::string(WIN_NAME);
}

/** Прорисовка матрицы img в окне  */
void Window::draw(const cv::Mat& img) {
    cv::imshow(WIN_NAME, img);
}


/*-------------------------  Main Window --------------------------------*/


MainWindow::MainWindow(Controller* controller) :  Window("SpectrMainWindow"), ptr_ctrl(controller) {
    cv::namedWindow(WIN_NAME, cv::WINDOW_AUTOSIZE | cv::WINDOW_GUI_EXPANDED);
    cv::setWindowTitle(WIN_NAME, std::string("Спектр ") + PROJECT_VERSION);
}


/** Отображение текста поверх окна на время mstime */
void MainWindow::overlayText(std::string text, int mstime) {
    cv::displayOverlay(WIN_NAME, text, mstime);
}


/** Создает элементы управления на панеле управления */

void MainWindow::create_controls(){
    
    cv::createButton("Видео",
        []([[maybe_unused]] int state, void* pctrl) {
            Controller* p = static_cast<Controller*>(pctrl);
            if (p){
                p->set_mode(Controller::mode::video);
            };
        }, static_cast<void*>(ptr_ctrl), cv::QT_PUSH_BUTTON
    );

    cv::createTrackbar("Усиление", std::string(), &ptr_ctrl->opt.gain, ptr_ctrl->glob_opt.gain_steps,
        [](int val, void* pctrl) {
            if (auto p = static_cast<Controller*>(pctrl)) {
                p->get_capture().set_property("GAIN", double(val * p->glob_opt.gain_step_val));
            };
        }, static_cast<void*>(ptr_ctrl)
    );

    cv::createTrackbar("Экспозиция", std::string(), &ptr_ctrl->opt.exposure,
                        ptr_ctrl->glob_opt.exposure_limit[1] - ptr_ctrl->glob_opt.exposure_limit[0],
        [](int val, void* pctrl) {
            if (auto p = static_cast<Controller*>(pctrl)) {
                p->get_capture().set_property("EXPOSURE", double(val + p->glob_opt.exposure_limit[0]));
            };
        }, static_cast<void*>(ptr_ctrl)
    );

    cv::createButton("Сетка", 
        []([[maybe_unused]] int state, void* pctrl) {
            if (auto p = static_cast<Controller*>(pctrl)) {
                p->showgrid(state);
            };
        }, static_cast<void*>(ptr_ctrl), cv::QT_CHECKBOX | cv::QT_NEW_BUTTONBAR, ptr_ctrl->opt.showgrid 
    );

    const int rot_range = 20;
    static int r = ptr_ctrl->opt.rotation + rot_range/2;
    cv::createTrackbar("Поворот", std::string(), &r, rot_range,
        [](int val, void* pctrl) {
            if (auto p = static_cast<Controller*>(pctrl)) {
                p->set_rotation(rot_range/2 - val);
            }
        }, static_cast<void*>(ptr_ctrl));

    cv::createButton("Задать окно",
        []([[maybe_unused]] int state, void* pctrl) {
            if (auto p = static_cast<Controller*>(pctrl)) {
                p->set_mode(Controller::mode::roi_selct);
            };
        }, static_cast<void*>(ptr_ctrl), cv::QT_PUSH_BUTTON | cv::QT_NEW_BUTTONBAR 
    );

    cv::createButton("Сбросить окно", 
        []([[maybe_unused]] int state, void* pctrl) {
            if (auto p = static_cast<Controller*>(pctrl)) {
                p->reset_roi();
            };
        }, static_cast<void*>(ptr_ctrl), cv::QT_PUSH_BUTTON 
    );

    cv::createButton("Спектр", 
        []([[maybe_unused]]int state, void* pctrl) {
            if (auto p = static_cast<Controller*>(pctrl)) {
                p->set_mode(Controller::mode::spectr);
            };
        }, static_cast<void*>(ptr_ctrl), cv::QT_PUSH_BUTTON | cv::QT_NEW_BUTTONBAR
    );

    cv::createButton("MS", 
        []([[maybe_unused]]int state, void* pctrl) {
            if (auto p = static_cast<Controller*>(pctrl)) {
                p->spectr_memset();
            };
        }, static_cast<void*>(ptr_ctrl), cv::QT_PUSH_BUTTON | cv::QT_NEW_BUTTONBAR
    );

    cv::createButton("MC",
        []([[maybe_unused]]int state, void* pctrl) {
            if (auto p = static_cast<Controller*>(pctrl)) {
                p->spectr_memclear();
            };
        }, static_cast<void*>(ptr_ctrl), cv::QT_PUSH_BUTTON 
    );

    cv::createButton("Экспорт спектра...", 
        []([[maybe_unused]]int state, void* pctrl) {
            if (auto p = static_cast<Controller*>(pctrl)) {
                p->export_spectr();
            };
        }, static_cast<void*>(ptr_ctrl), cv::QT_PUSH_BUTTON | cv::QT_NEW_BUTTONBAR 
    );

    cv::createTrackbar("Кадры накопления", std::string(), &ptr_ctrl->opt.spectr_acc_fps, 10,
        [](int val, void* pctrl) {
            if (auto p = static_cast<Controller*>(pctrl)) {
                p->set_spectr_fps(val);
            };
        }, static_cast<void*>(ptr_ctrl)
    );

    cv::createButton("Калибровка (сброс)", 
        []([[maybe_unused]]int state, void* pctrl) {
            if (auto p = static_cast<Controller*>(pctrl)) {
                p->reset_calibration() ;
            };
        }, static_cast<void*>(ptr_ctrl), cv::QT_PUSH_BUTTON | cv::QT_NEW_BUTTONBAR 
    );
    
    if (ptr_ctrl->glob_opt.calib_v[0] > 0) {
        cv::createButton(std::to_string(ptr_ctrl->glob_opt.calib_v[0])+" нм", []([[maybe_unused]]int state, void* pctrl) {
            if (auto p = static_cast<Controller*>(pctrl)) {
                p->calibrate(1);
            };
        }, static_cast<void*>(ptr_ctrl), cv::QT_PUSH_BUTTON | cv::QT_NEW_BUTTONBAR );
    }

    if (ptr_ctrl->glob_opt.calib_v[1] > 0) {
        cv::createButton(std::to_string(ptr_ctrl->glob_opt.calib_v[1])+" нм", []([[maybe_unused]]int state, void* pctrl) {
            if (auto p = static_cast<Controller*>(pctrl)) {
                p->calibrate(2);
            };
        }, static_cast<void*>(ptr_ctrl), cv::QT_PUSH_BUTTON );
    }

    if (ptr_ctrl->glob_opt.calib_v[2] > 0) {
        cv::createButton(std::to_string(ptr_ctrl->glob_opt.calib_v[2])+" нм", []([[maybe_unused]]int state, void* pctrl) {
            if (auto p = static_cast<Controller*>(pctrl)) {
                p->calibrate(3);
            };
        }, static_cast<void*>(ptr_ctrl), cv::QT_PUSH_BUTTON );
    }
}
