#pragma once

#include <iostream>
#include <map>
#include <cassert>

#include "Candlestick.h"
#include "MACD.h"
#define FMT_HEADER_ONLY
#include "fmt\format.h"

enum class TradeDataPeriod {
    Day,
    Minutes5,
    Minutes15,
    Minutes30
};
class Chart {
    std::vector<Candlestick> sticks_;
    std::map<std::string, std::vector<MACD>> macds_;
    std::map<std::string, std::vector<KDJ>> kdjs_;
public:
    Chart() {}
    void FillSticks(std::vector<Candlestick>& src) {
        sticks_ = src;
    }
    const std::vector<Candlestick>& sticks() const {
        return sticks_;
    }
    int GetIndex(const DateTime& time) const {
        for (size_t i = 0; i < this->sticks().size(); i++) {
            if (this->sticks()[i].trade_time == time)
                return i;
        }
        return -1;
    }
    double Hn(int data_i, int n) const {
        assert(this->sticks().size() > 1);
        double max = this->sticks()[data_i].high;
        int begin = data_i - n + 1;
        begin = MAX(0, begin);
        for (int i = begin; i <= data_i; i++ ) {
            if (this->sticks()[i].high > max) {
                max = this->sticks()[i].high;
            }
        }
        return max;
    }
    double Ln(int data_i, int n) const {
        assert(this->sticks().size() > 1);
        double min = this->sticks()[data_i].low;
        int begin = data_i - n + 1;
        begin = MAX(0, begin);
        for (int i = begin; i <= data_i; i++) {
            if (this->sticks()[i].low < min) {
                min = this->sticks()[i].low;
            }
        }
        return min;
    }
    std::vector<double> RawStochasticValues(int n) const {
        std::vector<double> rsvs;// = new std::vector<double>(this.tradeData.length);
        if (sticks_.size() == 0)
            return rsvs;
        rsvs.resize(this->sticks().size());

        for (size_t i = 0; i < this->sticks().size(); i++) {
            double hn = this->Hn(i, n);
            double ln = this->Ln(i, n);
            double C = this->sticks()[i].close;
            double rsv = (C - ln) / (hn - ln) * 100;
            if (hn - ln == 0.0)
                rsv = 0.0;
            if (rsv < 0) {
                // print("rsv < 0 at ${i} ${tradeData[i].tradeTime} high:${hn} low:${ln} C:${C}");
                assert(0 > 1);
            }
            rsvs[i] = rsv;
        }
        return rsvs;
    }
    const std::vector<MACD>& macd(int short_n, int long_n, int dea_n) {
        std::string id = fmt::format("MACD({0},{1},{2})", short_n, long_n, dea_n);
        if (macds_.find(id) == macds_.end()) {
            std::vector<MACD> macdnnn = MACDCalculator::GetResult(*this, short_n, long_n, dea_n);
            //macds_.insert(std::map<std::string, std::vector<MACD>>::value_type(id, macdnnn));
            macds_[id] = macdnnn;
            return macds_[id];
        } else {
            return macds_[id];
        }
    }
    const std::vector<KDJ>& kdj(int n, int sn1, int sn2) {
        std::string id = fmt::format("KDJ({0},{1},{2})", n, sn1, sn2);
        if (kdjs_.find(id) == kdjs_.end()) {
            std::vector<KDJ> kdjnnn = KDJCalculator::GetResult(*this, n, sn1, sn2 );
            //macds_.insert(std::map<std::string, std::vector<MACD>>::value_type(id, macdnnn));
            kdjs_[id] = kdjnnn;
            return kdjs_[id];
        } else {
            return kdjs_[id];
        }
    }
};

class Trade {
protected:
    std::map<TradeDataPeriod, Chart> charts_;
public:
    Trade() {
    }
    std::map<TradeDataPeriod, Chart>& charts() {
        return charts_;
    }
    void UpdateCharts() {
        if (charts_.find(TradeDataPeriod::Minutes5) != charts_.end()) {
            // Chart chart;
            std::vector<Candlestick> sticks15minutes;
            const std::vector<Candlestick>& sticks5min = this->charts()[TradeDataPeriod::Minutes5].sticks();
            for (size_t i = 0; i < sticks5min.size(); i += 3) {
                Candlestick newStick1 = Combine(sticks5min[i], sticks5min[i + 1]);
                Candlestick newStick = Combine(newStick1, sticks5min[i + 2]);
                sticks15minutes.push_back(newStick);
            }
            charts_[TradeDataPeriod::Minutes15].FillSticks(sticks15minutes);
        } else {
            assert(false);
        }
    }
};