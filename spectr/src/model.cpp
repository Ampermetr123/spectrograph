#include "model.h"
#include <numeric> //accumulate
#include "optlog.h"

/*---------------- Model --------------------------------*/

/** Подписать объект на изменение данных*/
void Model::subscribe(std::weak_ptr<ModelSubscriber> handler) {
    observers.push_back(handler);
};

/** Отписать объект  */
void Model::unsubscribe(std::weak_ptr<ModelSubscriber> handler) {
    observers.remove_if([h = handler](const std::weak_ptr<ModelSubscriber>& rv) {
        return h.lock() == rv.lock();
        }
    );
}

/** Уведомление для всех подписанных объектов */
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

const cv::Mat& Model_Video::get_data() {
    return data;
}


/*---------------- Model Spectr --------------------------------*/

/** Конструктор объекта
 * @param pts вектор с точками калибровки
 * @param accumulate_frames количество калров по которым накапливается спектр
 */
Model_Spectr::Model_Spectr(std::vector<std::pair<int, int>> pts, long accumulate_frames)
    : calibr_pts(pts), accumulate_frames(accumulate_frames) {
}

/** Установка количество калров по которым накапливается спектр */
void Model_Spectr::set_acc_fps(int fps) {
    accumulate_frames = fps;
}

/** Обработка очередного видеокадра */
void Model_Spectr::udpate_data(cv::Mat frame) {

    // Первый вызов функции 
    static int prev_frame_cols = -1;
    if (frame.cols != prev_frame_cols) {
        prev_frame_cols = frame.cols;
        data = cv::Mat::zeros(data_rows, frame.cols, CV_64F);
        spectr = cv::Mat::zeros(1, frame.cols, CV_64F);
        calibrate(calibr_pts);
        frames_counter = 0;
    }

    cv::Mat img;
    cv::cvtColor(frame, img, cv::COLOR_BGR2GRAY);
    for (int i = 0; i < img.cols; i++) {
        cv::Mat column = img.col(i);
        spectr.at<double>(0, i) += std::accumulate(column.begin<uchar>(), column.end<uchar>(), 0) / img.rows;
    }

    if (accumulate_frames != 0) {
        frames_counter = (frames_counter + 1) % accumulate_frames;
    }
    else {
        frames_counter = 0;
    }

    if (frames_counter == 0) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            memcpy(data.ptr<void>(row_base), spectr.ptr<void>(0), sizeof(double) * data.cols);

            // "Вычилсение Спектр" / "Сохраненный спектр"
            //  Закоментировано, так как пока не понятно, как лучше отображать резльтат и полезен ли такой расчет
            // if (mem_spectr) {
            //     memcpy(data.ptr<void>(row_bdm), spectr.ptr<void>(0), sizeof(double) * data.cols);
            //     auto bdm = data.ptr<double>(row_bdm);
            //     auto mem = data.ptr<double>(row_mem);
            //     for (int i = 0; i < data.cols; i++) {
            //         if (mem[i] > 1) {
            //             bdm[i] = bdm[i] / mem[i];
            //         } 
            //     }
            // }
        }
        spectr = cv::Mat::zeros(1, data.cols, CV_64F);
        notify();
    }
}

/** Сохранить (запомнить) текущий (последний накопленный) спектр */
void Model_Spectr::spectr_memset() {
    if (!data.empty()) {
        std::lock_guard<std::mutex> lock(mtx);
        memcpy(data.ptr<void>(row_mem), data.ptr<void>(row_base), sizeof(double) * data.cols);
        mem_spectr = true;
    }
}

/** Очистка сохраненного спектра */
void Model_Spectr::spectr_memclear() {
    mem_spectr = false;
}

/** Возваращает данные модели 
    Матрица в две строки - если нет сохраненного спектра
    Полная матрица - если есть сохраненный спектр
*/
const cv::Mat& Model_Spectr::get_data() {
    if (mem_spectr == false) {
        data_short = cv::Mat(data, cv::Range(0, 2));
        return data_short;
    }
    else {
        return data;
    }
}

/** Калибровка шкалы X по точкам
    Точка (позиция пикселя, длинна волны)
    Если в векторе 2 точки - то выполняется линейная интерполяция
    Если в векторе 3 точки - то выполняется интерполяция полиномом 2ой степени
    Если меньше одной точки, то y(x)=x; 
 */
void Model_Spectr::calibrate(const std::vector<std::pair<int, int>>& cpt) {
    calibr_pts = cpt;
    if (data.cols == 0)
        return;

    if (cpt.size() == 2) {   // linear interpolation
        double k = double(cpt[1].second - cpt[0].second) / (cpt[1].first - cpt[0].first);
        double b = ((cpt[1].second + cpt[0].second) - k * (cpt[1].first + cpt[0].first)) * 0.5;
        for (int i = 0; i < spectr.cols; i++) {
            data.at<double>(row_nm, i) = k * i + b;
        }
    }
    else if (cpt.size() >= 3) {   // 2nd inrerpolation
        auto x1 = cpt[0].first;
        auto x2 = cpt[1].first;
        auto x3 = cpt[2].first;
        auto y1 = cpt[0].second;
        auto y2 = cpt[1].second;
        auto y3 = cpt[2].second;
        double a = double((y3 - y1) * (x2 - x1) - (y2 - y1) * (x3 - x1)) / ((x3 * x3 - x1 * x1) * (x2 - x1) - (x2 * x2 - x1 * x1) * (x3 - x1));
        double b = (y2 - y1 - a * (x2 * x2 - x1 * x1)) / (x2 - x1);
        double c = y1 - (a * x1 * x1 + b * x1);
        for (int i = 0; i < spectr.cols; i++) {
            data.at<double>(row_nm, i) = a * i * i + b * i + c;
        }
    }
    else { // non calibrated scale
        for (int i = 0; i < spectr.cols; i++) {
            data.at<double>(row_nm, i) = i;
        }
    }
}
