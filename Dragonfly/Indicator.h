#pragma once

#include "Trade.h"
#include "Stock.h"

#define FMT_HEADER_ONLY
#include "../Dragonfly/fmt/format.h"

namespace indicator {
//inline double RSV(const Stock& stock, int i, int n) {
//    double hn = stock.Hn(i, n);
//    double ln = stock.Ln(i, n);
//    double C = stock.trade_data()[i].close;
//    double rsv = (C - ln) / (hn - ln) * 100;
//
//    if (rsv < 0) {
//        rsv = 0.0;
//        fmt::print("{0} {1} rsv < 0 C = {2} hn = {3} ln = {4}\n", stock.name(), stock.GetDate(i).ToString(), C, hn, ln);
//    }
//    //assert(stock.trade_data().size() != 1);
//    //assert(rsv >= 0 && stock.trade_data().size() != 1);
//    return rsv;
//}
// Raw Stochastic Value
//inline double RSV(const Stock& stock, eight_digit_time date, int n) {
//    int i = stock.GetIndex(date);
//    return RSV(stock, i, n);
//}
//
inline double StandardDeviation(const Chart& chart, int data_i, double u, int n) {
    //const TradeData* p = &d;
    double power_sum = 0.0;
    int i = 1;
    for (; i <= n; i++) {
        if (data_i - (i - 1) <= 0)
            break;
        const Candlestick* p = &(chart.sticks()[data_i - (i - 1)]);
        power_sum += (p->close - u) * (p->close - u);
    }
    return sqrt(power_sum / i);
}

inline double MA(const Chart& chart, int data_i, int n) {
    double ret = 0.0;
    for (int a = data_i; a > data_i - n && a >= 0; a--) {
        ret += chart.sticks()[a].close;
    }
    return ret / n;
}

//inline double MA(const TradeData& d, int n) {
//    double ret = 0.0;
//    const TradeData* p = &d;
//    int i = 0;
//    for (; i < n; i++) {
//        float c = p->close;
//        ret += c;
//        if (p->prev != nullptr) {
//            p = p->prev;
//        } else {
//            break;
//        }
//    }
//    return ret / i;
//}
//inline double MAVolume(const TradeData& d, int n) {
//    double ret = 0.0;
//    const TradeData* p = &d;
//    int i = 0;
//    for (; i < n; i++) {
//        float c = p->volume;
//        ret += c;
//        if (p->prev != nullptr) {
//            p = p->prev;
//        } else {
//            break;
//        }
//    }
//    return ret / i;
//}
inline void MA(double* des, const double* src, size_t n, size_t len) {
    for (size_t i = n - 1; i < len; i++) {
        des[i] = 0.0;
        for (size_t j = i; j > i - n; j--) {
            des[i] += src[j];
        }
        des[i] /= n;
    }
    for (size_t i = 0; i < n - 1 && i < len; i++) {
        des[i] = 0.0;
        for (int j = i; j > 0; j--) {
            des[i] += src[j];
        }
        des[i] /= (i + 1);
    }
}
inline void SUM(double* des, const double* src, size_t n, size_t len) {
    for (size_t i = n - 1; i < len; i++) {
        des[i] = 0.0;
        for (size_t j = i; j > i - n; j--) {
            des[i] += src[j];
        }
    }
    for (size_t i = 0; i < n - 1 && i < len; i++) {
        des[i] = 0.0;
        for (int j = i; j > 0; j--) {
            des[i] += src[j];
        }
    }
}
inline void SUM(float* des, const float* src, size_t n, size_t len) {
    for (size_t i = n - 1; i < len; i++) {
        des[i] = 0.0;
        for (size_t j = i; j > i - n; j--) {
            des[i] += src[j];
        }
    }
    for (size_t i = 0; i < n - 1 && i < len; i++) {
        des[i] = 0.0;
        for (int j = i; j > 0; j--) {
            des[i] += src[j];
        }
    }
}
// simple moving average
//inline std::vector<double> SMA(const Stock& s, double N, int M) {
//    double* sma = new double[s.trade_data().size()];
//    sma[0] = s.trade_data()[0].close;
//    for (size_t i = 1; i < s.trade_data().size(); i++) {
//        sma[i] = (s.trade_data()[i].close * M + sma[i - 1] * (N - M)) / N;
//    }
//    std::vector<double> ret;
//    for (size_t i = 0; i < s.trade_data().size(); i++) {
//        ret.push_back(sma[i]);
//    }
//    return ret;
//}
inline std::vector<double> EMA(const Chart& td, double N) {
    double* ema = new double[td.sticks().size()];
    ema[0] = td.sticks()[0].close;
    auto k = 2.0 / (N + 1.0);
    for (size_t i = 1; i < td.sticks().size(); i++) {
        ema[i] = td.sticks()[i].close * k + ema[i - 1] * (1 - k);
    }
    std::vector<double> ret;
    for (size_t i = 0; i < td.sticks().size(); i++) {
        ret.push_back(ema[i]);
    }
    return ret;
}
inline void EMA(double* ema, const double* src, size_t data_len, double N) {
    ema[0] = src[0];
    auto k = 2.0 / (N + 1.0);
    for (size_t i = 1; i < data_len; i++) {
        ema[i] = src[i] * k + ema[i - 1] * (1 - k);
    }
}
// simple moving average
// M weight ; N days
inline void SMA(double* sma, const double* src, int data_len, int N, double M) {
    sma[0] = src[0];
    // auto k = (double)M / (N + 1.0);
    double k = (double)M / N;
    for (int i = 1; i < data_len; i++) {
        sma[i] = src[i] * k + sma[i - 1] * (1 - k);
    }
}
template<typename T>
inline void SMA(std::vector<T>& smaDes, const std::vector<T>& src, int N, double M) {
    assert(smaDes.size() == src.size());
    smaDes[0] = src[0];
    double k = (double)M / N;
    for (size_t i = 1; i < src.size(); i++) {
        smaDes[i] = src[i] * k + smaDes[i - 1] * (1 - k);
    }
}
inline void SMA(std::vector<float>& smaDes, const std::vector<double>& src, int N, double M) {
    assert(smaDes.size() == src.size());
    smaDes[0] = src[0];
    double k = (double)M / N;
    for (size_t i = 1; i < src.size(); i++) {
        smaDes[i] = src[i] * k + smaDes[i - 1] * (1 - k);
    }
}

} // namespace indicator {

//【MA】：求简单移动平均
//用法：MA(X，N)，求X的N日移动平均值。
//算法：(X1 + X2 + X3 + ... + Xn) / N
//例如：MA(CLOSE，10) 表示求10日均价。特例：MA(X，0)表示X所有数据的平均。
//
//【EMA】：求指数平滑移动平均
//用法：EMA(X，N)，求X的N日指数平滑移动平均。
//算法：若Y = EMA(X，N)，则Y = [2 * X + (N - 1)*Y']/(N+1)，其中Y'表示上一周期Y值。
//例如：EMA(CLOSE，30) 表示求30日指数平滑均价。
//
//【SMA】：求移动平均
//用法：SMA(X，N，M)，求X的N日移动平均，M为权重。
//算法：若Y = SMA(X，N，M)，则 Y = [M*X + (N - M)*Y']/N，其中Y'表示上一周期Y值，N必须大于M。
//例如：SMA(CLOSE，30，1) 表示求30日移动平均价。
//
//【DMA】：求动态移动平均
//用法：DMA(X，A)，求X的A日动态移动平均。
//算法：若Y = DMA(X，A)，则 Y = A*X + (1 - A)*Y'，其中Y'表示上一周期Y值，A必须小于1。
//例如：DMA(CLOSE，VOL / CAPITAL) 表示求以换手率作平滑因子的平均价。