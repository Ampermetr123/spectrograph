/**
 * @file model.h
 * @author Sergey Simonov (sb.simonov@gmail.com)
 * @brief Модели данных 
 */

#pragma once
#include <memory>
#include <mutex>
#include <list>
#include "ocv.h"

class Model;

class ModelSubscriber {
public:
    virtual void on_data_updated(Model& m) = 0;
};


/** Базовый класс модели.
 * Интерфейс для загрузки и получения данных, реализация паттерна Observer (уведомление подписчиков об обновлении).
  */
class Model {
public:
    void subscribe(std::weak_ptr<ModelSubscriber> handler); 
    void unsubscribe(std::weak_ptr<ModelSubscriber> handler);
    void notify();
    virtual const cv::Mat& get_data() = 0;
    virtual void udpate_data(cv::Mat frame) = 0;
    virtual ~Model()=default;
protected:
    std::list<std::weak_ptr<ModelSubscriber>> observers;
};


/** Модель видеокадра */
class Model_Video : public Model {
public:
    const cv::Mat& get_data() override;
    void udpate_data(cv::Mat frame) override;
private:
    cv::Mat data;
};


/** Модель накопленного спектра */
class Model_Spectr : public Model {
public:
    enum {
        row_nm,
        row_base,
        row_mem,
        row_bdm, // "base spectr" div "mem spectr"
        data_rows
    };

    Model_Spectr(std::vector<std::pair<int, int>> cpt, long accumulate_frames);
    void udpate_data(cv::Mat frame) override;
    void calibrate(const std::vector<std::pair<int, int>> &cpt);
    void set_acc_fps(int fps);
    const cv::Mat& get_data() override;
    void spectr_memset();
    void spectr_memclear();

private:
    std::vector<std::pair<int, int>> calibr_pts;
    int accumulate_frames = 1;
    int frames_counter = 0;
    cv::Mat spectr, data, data_short;
    bool mem_spectr = false;
    std::mutex mtx;

};
