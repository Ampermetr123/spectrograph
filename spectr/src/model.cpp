#include "model.h"
#include <numeric> //accumulate
#include "optlog.h"

/*---------------- Model --------------------------------*/

cv::Mat& Model::get_data() {
    return data;
}

void Model::subscribe(std::weak_ptr<ModelSubscriber> handler) {
    observers.push_back(handler);
};

void Model::unsubscribe(std::weak_ptr<ModelSubscriber> handler) {
    observers.remove_if([h = handler](const std::weak_ptr<ModelSubscriber>& rv) {
        return h.lock() == rv.lock();
        }
    );
}

void Model::notify() {
    std::for_each(observers.begin(), observers.end(),
        [&](auto& s) {
            if (auto spt = s.lock()) {
                spt->on_data_updated(*this);
            };
        }
    );
}

/*---------------- Model Video --------------------------------*/

void Model_Video::udpate_data(cv::Mat frame) {
   
    data = frame;
    notify();
}


/*---------------- Model Spectr --------------------------------*/

Model_Spectr::Model_Spectr(double start, double delta, long accumulate_time_ms)
    : start_x(start), delta_x(delta), acc_time_ms(accumulate_time_ms) {
    start_tick = 0;
}


void Model_Spectr::udpate_data(cv::Mat frame) {

    static int prev_frame_cols = 0;
    if (frame.cols != prev_frame_cols) {
        prev_frame_cols = frame.cols;
        spectr = cv::Mat::zeros(2, frame.cols, CV_64F);
        for (int i = 0; i < frame.cols; i++) {
            spectr.at<double>(1, i) = start_x + i * delta_x;
        }
        start_tick = cv::getTickCount();
    }

    if (start_tick == 0) {
        start_tick = cv::getTickCount();
        spectr.row(0) = cv::Mat::zeros(1, spectr.cols, CV_64F);

    }

    cv::Mat img;
    cv::cvtColor(frame, img, cv::COLOR_BGR2GRAY);
    cv::Mat res(1, img.cols, CV_64F);
    for (int i = 0; i < img.cols; i++) {
        cv::Mat column = img.col(i);
        spectr.at<double>(0, i) += std::accumulate(column.begin<uchar>(), column.end<uchar>(), 0) / img.rows;
    }

    const double time_elapsed = 1000.0 * (cv::getTickCount() - start_tick) / cv::getTickFrequency();
    if (time_elapsed > acc_time_ms) {
        data = spectr.clone();
        start_tick = cv::getTickCount();
        spectr.row(0) = cv::Mat::zeros(1, spectr.cols, CV_64F);
        notify();
    }

}





