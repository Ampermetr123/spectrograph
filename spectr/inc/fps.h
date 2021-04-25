/**
 * @file fps.h
 * @author Sergey Simonov (sb.simonov@gmail.com)
 * @brief Класс для расчета FPS среднего за псоледнюю секунду
 */

#pragma once
#include <deque>
#include <iterator>
#include <algorithm>
#include <numeric>
#include "ocv.h"

class FPS_Stat {
    const int QSZ = 30;
    std::deque<int64_t> ticks;
    static double diff_ms(int64_t t1, int64_t t2) {
        return 1000.0 * (t2-t1) / cv::getTickFrequency();
    }
public:

    FPS_Stat() = default;

    /** Уведомление о наступлении события (кадра) */
    void tick() {
        ticks.push_back(cv::getTickCount());
        if (ticks.size() > QSZ) {
            ticks.pop_front();
        }
    }

    /** Возращает усредненную частоту событий за поуледнюю секнду */
    double fps() {
        auto now = cv::getTickCount();
        while (ticks.size() && diff_ms(ticks.front(), now) > 1000)
            ticks.pop_front();
        if (ticks.size() == 0) {
            return 0;
        }
        else if (ticks.size() == 1) {
            return diff_ms(now, ticks.front());
        }
        std::vector<double> diffs;
        diffs.reserve(ticks.size());
        std::transform(std::next(ticks.begin()), ticks.end(), ticks.begin(), std::back_inserter(diffs),
            [](const int64_t& t2, const int64_t t1) { return FPS_Stat::diff_ms(t1, t2);});
        return 1000 / (std::accumulate(diffs.begin(), diffs.end(), 0.0) / diffs.size());
    }
};