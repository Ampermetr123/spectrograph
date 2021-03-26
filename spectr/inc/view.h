#pragma once
#include "ocv.h"
#include "model.h"

#include "window.h"


class View : public ModelSubscriber {
protected:
    Window& window;
public:
    View(Window& w) : window(w) {};
};

class VideoView : public View {
public:
    VideoView(Window& w) : View(w) {};
    void on_data_updated(Model& m) override {
        window.draw(m.get_data());
    };
};


class PlotView : public View {
public:
    PlotView(Window& w) : View(w) {
    }

    void on_data_updated(Model& m) override {
        cv::Mat data = m.get_data();
        cv::Ptr<cv::plot::Plot2d> plot = cv::plot::Plot2d::create(data.row(1), data.row(0));
        cv::Mat plot_result;
        plot->setPlotSize(data.cols, 768);
        plot->setShowText( true );
        plot->setShowGrid( true );
        plot->setPlotBackgroundColor( cv::Scalar( 255, 200, 200 ) );
        plot->setPlotLineColor( cv::Scalar( 255, 0, 0 ) );
        plot->setPlotLineWidth(2);
        plot->setInvertOrientation(true);
        plot->render(plot_result);
        window.draw(plot_result);
       // imwrite("plot.jpg", plot_result); 
    }
};
    

