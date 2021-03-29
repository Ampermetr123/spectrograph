#pragma once

#include <memory>
#include <list>
#include "ocv.h"

class Model;

class ModelSubscriber {
public:
    virtual void on_data_updated(Model& m) = 0;
};


class Model {
public:
    cv::Mat& get_data();

    cv::Rect get_ROI();
    void subscribe(std::weak_ptr<ModelSubscriber> handler);
    void unsubscribe(std::weak_ptr<ModelSubscriber> handler);
    void notify();
    virtual void udpate_data(cv::Mat frame) = 0;
    virtual void set_ROI(cv::Rect rect);
    virtual ~Model()=default;
protected:
    cv::Rect roi;
    cv::Mat data;
    std::list<std::weak_ptr<ModelSubscriber>> observers;
};






class Model_Video : public Model {
public:
    void udpate_data(cv::Mat frame) override;
};


class Model_Spectr : public Model {
    double start_x;
    double delta_x;
    long acc_time_ms;
    cv::Mat spectr;
    cv::int64_t start_tick;
    cv::Rect roi;
public:
    Model_Spectr(double start, double delta, long accumulate_time_ms);
    void set_ROI(cv::Rect rect) override;
    void udpate_data(cv::Mat frame) override;

};

class Model_ROI_Select : public Model {
    cv::Rect roi;
public:
    void udpate_data(cv::Mat frame) override;
    
};


