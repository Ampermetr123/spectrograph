/**
 * @file controller.cpp
 * @author Sergey Simonov (sb.simonov@gmail.com)
 */
#include <memory>
#include <fstream>
#include "controller.h"
#include "helpers.h"
#include "window.h"
#include "view.h"
#include "optlog.h"
#include "save_dialog.h"
#include "format.h"

/** Контсруктор объекта
 *  @param config_file путь к файлу с глобальными настройками программы
 */
Controller::Controller(const std::string& config_file) : config_file(config_file) {
    cv::FileStorage fs;
    if (!fs.open(config_file, cv::FileStorage::READ | cv::FileStorage::FORMAT_YAML)) {
        throw std::runtime_error(std::string("Can't open config file ") + config_file);
    }
    glob_opt.calib_v[0] = load_or_default(fs, "spectr", "CALIB_L1", glob_opt.calib_v[0]);
    glob_opt.calib_v[1] = load_or_default(fs, "spectr", "CALIB_L2", glob_opt.calib_v[1]);
    glob_opt.calib_v[2] = load_or_default(fs, "spectr", "CALIB_L3", glob_opt.calib_v[2]);
    glob_opt.spectr_win_width = load_or_default(fs, "spectr", "WIN_WIDTH", glob_opt.spectr_win_width);
    glob_opt.spectr_win_height = load_or_default(fs, "spectr", "WIN_HEIGHT", glob_opt.spectr_win_height);
    glob_opt.gain_step_val = load_or_default(fs, "control", "GAIN_STEP_VAL", glob_opt.gain_step_val);
    glob_opt.gain_steps = load_or_default(fs, "control", "GAIN_STEPS", glob_opt.gain_steps);
    glob_opt.exposure_limit[0] = load_or_default(fs, "control", "EXPOSURE_LIMIT_LOW", glob_opt.exposure_limit[0]);
    glob_opt.exposure_limit[1] = load_or_default(fs, "control", "EXPOSURE_LIMIT_HIGHT", glob_opt.exposure_limit[0]);
    
    capture = Capture::create(fs);
    if (!capture || capture->width() == 0 || capture->height() == 0) {
        throw std::runtime_error("Bad capture device (width or height of frame is 0)");
    }

    options_file = get_user_dir() + std::string("\\spectr.options.yml");
    log1 << "Reading local user options from " << options_file;
    opt.load(options_file);
    rotation = opt.rotation;
    model_video = std::make_unique<Model_Video>();
    model_spectr = std::make_unique<Model_Spectr>(opt.calib_points(), opt.spectr_acc_fps);
    win_main = std::make_unique<MainWindow>(this);
    view_video = std::make_shared<VideoView>(*win_main);
    view_video->showgrid(opt.showgrid);
    model_video->subscribe(view_video);
    view_spectr = std::make_shared<SpectrView>(*win_main, glob_opt.spectr_win_width, glob_opt.spectr_win_height);
    model_spectr->subscribe(view_spectr);
}


Controller::~Controller() {
    opt.save(options_file);
}


Capture& Controller::get_capture() {
    return *capture;
}


/** Основной цикл приложения */
int Controller::run() {
    cv::Mat frame;
    win_main->create_controls();
    set_mode(mode::video);
   
    while (win_main->visible()) {
        capture->read(frame);
        cv::Mat filtered_frame;

        if (current_mode == mode::roi_selct) {
            set_mode(mode::video);
            win_main->overlayText("Задайте окно с помощью мыши и нажмите [Пробел] или [Ввод]", 0);
            opt.set_roi(cv::selectROI(win_main->name(), frame, true));
            log1 << "selected roi=" << opt.roi() << std::endl;
            current_mode = mode::video;
            win_main->overlayText("  ", 10);
            continue;
        }

        auto roi = opt.roi();
        if (rotation != 0 && roi == cv::Rect()) {
            // Поворот всего кадра
            cv::Mat r = cv::getRotationMatrix2D(cv::Point2f(frame.cols / 2.F, frame.rows / 2.F), rotation, 1.0);
            cv::warpAffine(frame, filtered_frame, r, frame.size()); 
        }
        else if (rotation != 0 && roi != cv::Rect()) {
            // Поворот окна в кадре - выбираем область кадра вокруг окна (roi), чтобы после поворота не было черных полей
            auto center_point = (roi.br() + roi.tl()) * 0.5;
            auto brect = cv::RotatedRect(center_point, roi.size(), static_cast<float>(rotation)).boundingRect();
            cv::Rect frect(cv::Point(max(0, min(roi.tl().x, brect.tl().x) ),  max(0, min(roi.tl().y, brect.tl().y))),
                         cv::Point(min(frame.cols, (roi.br().x, brect.br().x)), min(frame.rows, max(roi.br().y, brect.br().y))) 
            );
            auto subframe = cv::Mat(frame, frect);
            cv::Mat r = cv::getRotationMatrix2D(cv::Point2f(subframe.cols / 2.F, subframe.rows / 2.F), rotation, 1.0);
            cv::warpAffine(subframe, filtered_frame, r, subframe.size());
            cv::Rect roi_in_subframe( (frect.width-roi.width)/2, (frect.height-roi.height)/2, roi.width, roi.height);
            filtered_frame = cv::Mat(filtered_frame, roi_in_subframe);
        }
        // Окно в кадре без поворотов
        else if (roi != cv::Rect()) {
            filtered_frame = cv::Mat(frame, roi);
        }
        else {
            filtered_frame = frame;
        }
       

        if (current_mode == mode::spectr) {
            model_spectr->udpate_data(filtered_frame);
        }
        else if (current_mode == mode::video) {
            model_video->udpate_data(filtered_frame);
        }

        int key = cv::waitKey(1);
        if (key == ' ') {
            set_mode(current_mode == mode::spectr ? mode::video : mode::spectr);
        }
        // else if (key == 'r') {
        //     set_mode(mode::roi_selct);
        // } 
        else if (key>0) {
            log1 << "Key pressed code "<< key<<std::endl;
        };
        
    }
    return 0;
}


void Controller::set_mode(mode m) {
    log1 << "set__mode" << (int)m << std::endl;
    if (m == mode::spectr) {
        view_spectr->activate();
        view_video->deactivate();
    }
    else {
        view_spectr->deactivate();
        view_video->activate();
      
    };
    current_mode.store(m);
}


void Controller::reset_roi() {
    opt.set_roi(cv::Rect());
}


void Controller::spectr_memset() {
    model_spectr->spectr_memset();
}


void Controller::spectr_memclear() {
    model_spectr->spectr_memclear();
}


void Controller::showgrid(int state) {
    opt.showgrid = state;
    view_video->showgrid(state);
}


void Controller::set_spectr_fps(int fps) {
    opt.spectr_acc_fps = fps;
    model_spectr->set_acc_fps(fps);
}


void Controller::calibrate(int n)
{
    current_mode = mode::spectr;
    auto callback = [this, n](int x) {
        int i = n - 1; 
        if (i >= 0 || i<3 ) {
            opt.calib_x[i] = x;
            opt.calib_v[i] = glob_opt.calib_v[i];
            log1 << "calib point " << i << " " << x << " " << glob_opt.calib_v[i] << std::endl;
        }
        model_spectr->calibrate(opt.calib_points());
        
    };
    view_spectr->pick_X(callback);
}


void Controller::reset_calibration() {
    for (int i=0; i<3; i++){
        opt.calib_x[i] = -1;
        opt.calib_v[i] = glob_opt.calib_v[i];         
    }
    model_spectr->spectr_memclear();
    model_spectr->calibrate(opt.calib_points());
}


void Controller::export_spectr() {
    if (current_mode != Controller::mode::spectr) {
        win_main->overlayText("Для экспорта спектра включите отображение спектра", 3000);
        return;
    }
    auto path = save_file_dialog();
    if (path.empty())
        return;
    std::ofstream f(path);
    auto mat = model_spectr->get_data();
    if (mat.rows <= 2) {
        f << "nm;arb.u\n";
        for (int i = 0; i < mat.cols; i++) {
            f << fmt::format("{:.2f};{:.1f}\n", mat.at<double>(0, i), mat.at<double>(1, i));
        }
    }
    else {
        f << "nm;arb.u;arb.u(mem)\n";
        for (int i = 0; i < mat.cols; i++) {
            f << fmt::format("{:.2f};{:.1f};{:.1f}\n", mat.at<double>(0, i), mat.at<double>(1, i), mat.at<double>(2, i));
        }
    }
}

void Controller::set_rotation(int angle) {
        opt.rotation = rotation = angle;
}


/*-------------------- Сontroller::Options  -----------------------------------------*/

/** Сохранение локальных настроек в файл  */
void Controller::Options::load(std::string file_path) {
    cv::FileStorage fs(file_path, cv::FileStorage::READ);
    roi_x = load_or_default(fs, "ROI", "x", roi_x);
    roi_y = load_or_default(fs, "ROI", "y", roi_y);
    roi_width = load_or_default(fs, "ROI", "width", roi_width);
    roi_height = load_or_default(fs, "ROI", "height", roi_height);
    exposure = load_or_default(fs, "CAMERA", "exposure", exposure);
    gain = load_or_default(fs, "CAMERA", "gain", gain);
    rotation = load_or_default(fs, "CAMERA", "rotation", rotation);
    showgrid = load_or_default(fs, "CAMERA" , "showgrid", showgrid);
    spectr_acc_fps = load_or_default(fs, "", "spectr_acc_fps", spectr_acc_fps);
    calib_x[0] = load_or_default(fs, "CALIBRATION", "calib_x1", calib_x[0]);
    calib_v[0] = load_or_default(fs, "CALIBRATION", "calib_v1", calib_v[0]);
    calib_x[1] = load_or_default(fs, "CALIBRATION", "calib_x2", calib_x[1]);
    calib_v[1] = load_or_default(fs, "CALIBRATION", "calib_v2", calib_v[1]);
    calib_x[2] = load_or_default(fs, "CALIBRATION", "calib_x3", calib_x[2]);
    calib_v[2] = load_or_default(fs, "CALIBRATION", "calib_v3", calib_v[2]);
}


/** Загрузка локальных настроек из файла  */
void Controller::Options::save(std::string file_path) {
    cv::FileStorage fs(file_path, cv::FileStorage::WRITE);
    fs.startWriteStruct("CAMERA",cv::FileNode::MAP);
    fs.write("gain", gain);
    fs.write("exposure", exposure);
    fs.write("rotation", rotation);
    fs.write("showgrid", showgrid);
    fs.endWriteStruct();

    fs.startWriteStruct("ROI",cv::FileNode::MAP);
    fs.write("x", roi_x);
    fs.write("y", roi_y);
    fs.write("width", roi_width);
    fs.write("height", roi_height);
    fs.endWriteStruct();

    fs.write("spectr_acc_fps", spectr_acc_fps);

    fs.startWriteStruct("CALIBRATION",cv::FileNode::MAP);
    fs.write("calib_x1", calib_x[0]);
    fs.write("calib_v1", calib_v[0]);
    fs.write("calib_x2", calib_x[1]);
    fs.write("calib_v2", calib_v[1]);
    fs.write("calib_x3", calib_x[2]);
    fs.write("calib_v3", calib_v[2]);        
    fs.endWriteStruct();
}


cv::Rect Controller::Options::roi() {
    return cv::Rect(roi_x,roi_y,roi_width,roi_height);
}


void Controller::Options::set_roi(cv::Rect r) {
    roi_x = r.x;
    roi_y = r.y;
    roi_width = r.width;
    roi_height = r.height;
}


/** Возвращает вектор точек калибровки
 *  Отбрасывает точки с повторяющимся значением первой координаты
 *  и с отрицательными значениями координат (означает, что точка не задана)
 * @return std::vector<std::pair<int, int>> 
 */
std::vector<std::pair<int, int>> Controller::Options::calib_points() {
    std::vector < std::pair<int, int>> pts;
    for (int i=0; i<3; i++){
        if (calib_v[i] > 0 && calib_x[i] >= 0) {
            bool dublicate_x = false;
            for (int j = 0; j < i; j++) {
                if (calib_x[j] == calib_x[i]) {
                    dublicate_x = true;
                }
            }
            if (!dublicate_x) {
                pts.push_back({ calib_x[i], calib_v[i] });
            }
        }
    }
    return pts;
}
