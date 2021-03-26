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

void Model_Spectr::setROI(cv::Rect rect) {
    roi = rect;
    spectr = cv::Mat::zeros(2, rect.width, CV_64F);
    for (int i = 0; i < rect.width; i++) {
        spectr.at<double>(1, i) = start_x + i * delta_x;
    }
    start_tick = cv::getTickCount();
}

void Model_Spectr::udpate_data(cv::Mat frame) {
    // log1<<"s";
    // std::cout.flush();
    cv::Mat img;
    if (roi == cv::Rect()) {
        this->setROI(cv::Rect(0, 0, frame.cols, frame.rows));
    }

    if (frame.cols < roi.width || frame.rows < roi.height) {
        this->setROI(cv::Rect(0, 0, cv::min(frame.cols, roi.width), cv::min(frame.rows, roi.height)));
    }


    if (start_tick == 0) {
        start_tick = cv::getTickCount();
        spectr.row(0) = cv::Mat::zeros(1, spectr.cols, CV_64F);

    }

    cv::cvtColor(cv::Mat(frame, roi), img, cv::COLOR_BGR2GRAY);
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



