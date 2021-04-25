/**
 * @file view.cpp
 * @author Sergey Simonov (sb.simonov@gmail.com)
 */
#include <algorithm>
#include <functional>
#include "view.h"
#include "format.h"
#include "fps.h"
#include "optlog.h"

/*---------------------------- VideoView -------------------------------------*/

/** Конструктор
 * @param w - Объект окна, на котором работает вид 
 */
VideoView::VideoView(Window& w) : View(w) {
}

/** Отбражение видеокадра, поступившего от модели*/
void VideoView::on_data_updated(Model& m) {
    auto& frame = m.get_data();
    if (frame.empty()) {
        return;
    }

    if (add_grid) {
        const int n = 4; 
        for (int i = 1; i < n; i ++) {
            cv::line(frame, cv::Point(0, frame.rows * i/n), cv::Point(frame.cols - 1, frame.rows * i/n),
                cv::Scalar(0, 100, 200), 1, cv::LineTypes::LINE_4);
            cv::line(frame, cv::Point(frame.cols * i/n, 0), cv::Point(frame.cols * i/n, frame.rows),
                cv::Scalar(0, 100, 200), 1, cv::LineTypes::LINE_4);
        }
    }
    fps.tick();
    window.draw(frame);
    std::string str = fmt::format("Frame size: {}x{} \t | FPS: {:.1f}", frame.cols, frame.rows, fps.fps());
    cv::displayStatusBar(window.name(), str, 0);
};

/** Управление режимом отображения спектра*/
void VideoView::showgrid(bool state) {
    add_grid = state;
}


/*---------------------------- SpectrView -------------------------------------*/

/** Конструктор
 * @param w - Объект окна Window, на котором работает вид 
 * @param width - Ширина окна
 * @param height - Высота окна
 */
SpectrView::SpectrView(Window& w, int width, int height) :
    View(w), width(width), height(height) {
    using namespace CvPlot;
    using namespace std::placeholders;
    axes.create<Border>();
    auto& xAxis = axes.create<XAxis>();
    auto& yAxis = axes.create<YAxis>();
    axes.create<VerticalGrid>(&xAxis);
    axes.create<HorizontalGrid>(&yAxis);
    axes.create<CvPlot::Series>("-b").setName("spectr_base");
  
    axes.xLabel("nm");
    axes.yLabel("arb.u");
    axes.create<VerticalLine>(-1000, "-k").setName("vline");
    axes.setYLimAuto(false);
    axes.setXTight(true);
    axes.setYTight(true);
}


/** Активация нужна для разделения видами одного окна Window
 *  Акиивация означает, что в окне принадлежит текущему виду
 */
void SpectrView::activate() {
    View::activate();
    cv::setMouseCallback(window.name(), SpectrView::mouse_handler, this);
}


/** Обработчик событий мыши в окне
 * @param event тип события 
 * @param x позиция курсора
 * @param y позиция курсора 
 * @param flags - не испльзуется
 * @param userdata указатель на объект SpectrView (на себя)
 */
void SpectrView::mouse_handler(int event, int x, int y, int flags, void* userdata) {
    using namespace CvPlot;
    SpectrView& view = *static_cast<SpectrView*>(userdata);
    MouseEvent mouseEvent(view.axes, cv::Size2i(view.width, view.height), event, x, y, flags);
    if (view.is_active == false) {
        return;
    }

    if (view.pick_X_mode == false) {
        if (event == cv::EVENT_MOUSEMOVE) {
            auto x1 = mouseEvent.projection().innerRect().x;
            auto x2 = x1 + mouseEvent.projection().innerRect().width;
            auto y1 = mouseEvent.projection().innerRect().y;
            auto y2 = y1 + mouseEvent.projection().innerRect().height;
            if (x >= x1 && x <= x2 && y >= y1 && y <= y2) {
                view.mouse_pos_x = static_cast<int>(mouseEvent.pos().x);
                view.mouse_pos_y = static_cast<int>(mouseEvent.pos().y);
            }
            else {
                view.mouse_pos_x = view.mouse_pos_y = -1;
            }
        }
    }
    else {
        if (event == cv::EVENT_MOUSEMOVE) {
            auto vline = view.axes.find<VerticalLine>("vline");
            vline->setPos(mouseEvent.pos().x);
            auto plot_result = view.axes.render(view.height, view.width);
            view.window.draw(plot_result);
        }
        else if (event == cv::EVENT_LBUTTONUP) {
            auto vline = view.axes.find<VerticalLine>("vline");
            vline->setPos(-1000); // способ скрыть линию
            auto plot_result = view.axes.render(view.height, view.width);
            view.window.draw(plot_result);
            if (view.pick_X_callback) {
                auto data_x_size = view.axes.find<Series>("spectr_base")->getX().size();
                auto render_x_size = mouseEvent.projection().innerRect().width;
                auto px = mouseEvent.innerPoint().x;
                if (px < 0) {
                    px = 0;
                }
                if (px > render_x_size - 1) {
                    px = render_x_size - 1;
                }
                int x_norm = static_cast<int> (px * data_x_size / render_x_size);
                view.pick_X_callback(x_norm);
            }
            view.pick_X_mode = false;
        }
    }
}


/**
 * Включить режим выбора координаты X c помощью мыши 
 * @param callback Функция обработчик выбранного значения
 */
void SpectrView::pick_X(std::function<void(int x)> callback) {
    if (pick_X_mode == false) {
        pick_X_callback = callback;
        pick_X_mode = true;
    }
}

/** Обработка новой порции данных от модели*/
void SpectrView::on_data_updated(Model& m) {
    using namespace CvPlot; 
    fps.tick();
    cv::Mat data = m.get_data();
    if (data.empty()) {
        return;
    }

    const bool have_mem_spectr = data.rows > 2;
    auto xData = data.row(Model_Spectr::row_nm);
    auto yData = data.row(Model_Spectr::row_base);
    auto ser1 = axes.find<Series>("spectr_base");
    ser1->setX(xData);
    ser1->setY(yData);

    // Графики для дополнительных спектров создаются и удалются динамически
    Series* ser2 = axes.find<Series>("spectr_mem");
    Series* ser3 = nullptr;
    //Series* ser3 = axes.find<Series>("spectr_bdm");
    if (have_mem_spectr && ser2 == nullptr) {
        axes.create<CvPlot::Series>("-g").setName("spectr_mem").setX(data.row(Model_Spectr::row_nm)).setY(data.row(Model_Spectr::row_mem));
        mem_spectr_y_limits = calc_limit(data.row(Model_Spectr::row_mem));
        // axes.create<CvPlot::Series>("--r").setName("spectr_bdm").setX(data.row(Model_Spectr::row_nm)).setY(data.row(Model_Spectr::row_bdm));
    }
    // else if (ser3 != nullptr) {
    //     ser3->setX(data.row(Model_Spectr::row_nm));
    //     ser3->setY(data.row(Model_Spectr::row_bdm));
    // }
    else if (!have_mem_spectr && ( ser2 != nullptr || ser3 != nullptr)) {
        std::erase_if(axes.drawables(), [ser2, ser3](const std::unique_ptr<Drawable>& drw) {return (drw.get() == ser2 || drw.get()==ser3);});
    }

    // Настройка шкалы Y по отображаемым данным
    auto lim = calc_limit(yData);
    if (have_mem_spectr) {
        lim.first = std::min(lim.first, mem_spectr_y_limits.first);
        lim.second = std::max(lim.second, mem_spectr_y_limits.second);
    };
    if (abs(axes.getYLim().first - lim.first) > Y_LIM_STEP / 2 ||
         abs(axes.getYLim().second - lim.second) > Y_LIM_STEP / 2) {
        axes.setYLim(lim);
    }

    // Отображение полотна
    cv::Mat plot_result = axes.render(height, width);
    window.draw(plot_result);

    // Отображение строки состояния
    if (is_active) {
        if (mouse_pos_y != -1) {
            std::string str = fmt::format("({} nm, {} arb.u) | FPS: {:.1f}", mouse_pos_x, mouse_pos_y, fps.fps());
            cv::displayStatusBar(window.name(), str, 0);
        }
        else {
            std::string str = fmt::format("FPS: {:.1f}", fps.fps());
            cv::displayStatusBar(window.name(), str, 0);
        }
    }
}


/** Возвращает значения мнимального и максимального занчения для шкалы Y, чтобы вместить график
 * @param yData Матрица с одной строкой (значения координаты y)
  */
std::pair<double, double> SpectrView::calc_limit(const cv::Mat &yData) {
    std::pair<double, double> lim{ *std::min_element(yData.begin<double>(), yData.end<double>()) - Y_LIM_STEP / 2 ,
                                   *std::max_element(yData.begin<double>(), yData.end<double>()) + Y_LIM_STEP / 2 };
    lim.first = (int(lim.first) / Y_LIM_STEP) * Y_LIM_STEP;
    lim.second = (int(lim.second) / Y_LIM_STEP + 1) * Y_LIM_STEP;
    return lim;
}