#pragma once

#include "Stock.h"

#define FMT_HEADER_ONLY
#include "../Dragonfly/fmt/format.h"

namespace indicator {
inline double RSV(const Stock& stock, int i, int n) {
    double hn = stock.Hn(i, n);
    double ln = stock.Ln(i, n);
    double C = stock.trade_data()[i].close;
    double rsv = (C - ln) / (hn - ln) * 100;

    if (rsv < 0) {
        rsv = 0.0;
        fmt::print("{0} {1} rsv < 0 C = {2} hn = {3} ln = {4}\n", stock.name(), stock.GetDate(i).ToString(), C, hn, ln);
    }
    //assert(stock.trade_data().size() != 1);
    //assert(rsv >= 0 && stock.trade_data().size() != 1);
    return rsv;
}
// Raw Stochastic Value
inline double RSV(const Stock& stock, eight_digit_time date, int n) {
    int i = stock.GetIndex(date);
    return RSV(stock, i, n);
}

inline double StandardDeviation(const TradeData& d, double u, int n) {
    const TradeData* p = &d;
    double power_sum = 0.0;
    int i = 1;
    for (; i <= n; i++) {
        power_sum += (p->close - u) * (p->close - u);
        if (p->prev != nullptr) {
            p = p->prev;
        } else {
            break;
        }
    }
    return sqrt(power_sum / i);
}

inline double MA(const Stock& s, int i, int n) {
    double ret = 0.0;
    for (int a = i; a > i - n; a--) {
        ret += s.trade_data()[a].close;
    }
    return ret / n;
}

inline double MA(const TradeData& d, int n) {
    double ret = 0.0;
    const TradeData* p = &d;
    int i = 0;
    for (; i < n; i++ ) {
        float c = p->close;
        ret += c;
        if (p->prev != nullptr) {
            p = p->prev;
        } else {
            break;
        }
    }
    return ret / i;
}
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
inline std::vector<double> EMA(const Stock& s, double N) {
    double* ema = new double[s.trade_data().size()];
    ema[0] = s.trade_data()[0].close;// 第一天ema等于当天收盘价
    auto k = 2.0 / (N + 1.0);// 计算出序数
    for (size_t i = 1; i < s.trade_data().size(); i++) {
        // 第二天以后，当天收盘 收盘价乘以系数再加上昨天EMA乘以系数-1
        ema[i] = s.trade_data()[i].close * k + ema[i - 1] * (1 - k);
    }
    std::vector<double> ret;
    for (size_t i = 0; i < s.trade_data().size(); i++) {
        ret.push_back(ema[i]);
    }
    return ret;
}
// simple moving average
inline std::vector<double> SMA(const Stock& s, double N, int M) {
    double* sma = new double[s.trade_data().size()];
    sma[0] = s.trade_data()[0].close;
    for (size_t i = 1; i < s.trade_data().size(); i++) {
        sma[i] = (s.trade_data()[i].close * M + sma[i - 1] * (N - M)) / N;
    }
    std::vector<double> ret;
    for (size_t i = 0; i < s.trade_data().size(); i++) {
        ret.push_back(sma[i]);
    }
    return ret;
}
}