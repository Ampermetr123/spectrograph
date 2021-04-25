/**
 * @file capture.h
 * @author Sergey Simonov (sb.simonov@gmail.com)
 * @brief Классы видеозахвата. Обертка над cv::VideoCapture
 */

#pragma once
#include <string>
#include <memory>
#include <map>
#include "ocv.h"

/** Базовый класс видеозахвата
*   + Реализует статичный метод создания объекта видеозахвата в соответсвии с параметром capture_options:source
*/
class Capture {
protected:
    cv::VideoCapture cap;
    static const std::map<const std::string, long> cap_props;
public:
    static std::unique_ptr<Capture> create(cv::FileStorage& fs);
    void set_property(std::string prop, double value);
    double get_property(std::string prop);
    int height();
    int width();
    virtual void read(cv::Mat& frame) = 0;
    virtual ~Capture() = default;
};


/** Видеозахват от устройства (видеокамеры) */
class CameraCapture : public Capture {
public:
    CameraCapture(int devNo, cv::FileStorage& config);
    void read(cv::Mat& frame) override;
};


/** Видеозахват из папки с изображениями
 *  Имена файлов должны иметь формат img_%02d.jpg. Проигрывает файлы по кругу.
 *  Частота выдачи кадров задается параметром capture_options:FPS в файле конфигурации.
*/
class FileCapture : public Capture {
    int delay_ms = 1000;
    const int64_t ticks_in_ms;
    int64_t grab_time = 0;

public:   
    FileCapture(std::string path, cv::FileStorage& config);
    void read(cv::Mat& frame) override;
};
