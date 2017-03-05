#pragma once

#include <iostream>
#include <map>
#include <cassert>

#include "Candlestick.h"
#include "MACD.h"
#define FMT_HEADER_ONLY
#include "fmt\format.h"

class Chart {
private:
    std::vector<Candlestick> sticks_;
    //Candlestick* psticks_ = nullptr;
    std::map<std::string, std::vector<MACD>> macds_;
    std::vector<std::unique_ptr<BOLL>> bolls_;
    std::vector<std::unique_ptr<POI>> pois_;
public:
    std::vector<std::unique_ptr<KDJ>> kdjs_;
public:
    // std::vector<std::vector<MACD>*> qmacds;
    //std::vector<std::unique_ptr<KDJ>> qkdjs;
public:
    //std::map<int, std::vector<double>> rsvs;
    Chart() {}
    Chart(const std::vector<Candlestick>& src) {
        std::copy(src.begin(), src.end(), back_inserter(sticks_));
        // psticks_ = new Candlestick[sticks_.size()];
        // for (int i = 0; i < sticks_.size(); i++) {
        //     psticks_[i] = sticks_[i];
        // }
    }
    ~Chart() {
        //DELETE_IF_NOT_NULLPTR(psticks_);
    }
    const std::vector<Candlestick>& sticks() const {
        return sticks_;
    }
    //const Candlestick* psticks() const {
    //    return psticks_;
    // }
    const std::vector<std::unique_ptr<KDJ>>& kdjs() const {
        return kdjs_;
    }
    const std::vector<std::unique_ptr<BOLL>>& bolls() const {
        return bolls_;
    }
    int GetIndex(const DateTime& time) const {
        for (size_t i = 0; i < this->sticks().size(); i++) {
            if (this->sticks()[i].trade_time == time)
                return i;
        }
        return -1;
    }
    int GetTimeIndex(const DateTime& time) const {
        for (size_t i = 0; i < this->sticks().size(); i++) {
            //std::cout << time.ToString() << std::endl;
            // std::cout << this->sticks()[i].trade_time.ToString() << std::endl;
            if (time <= this->sticks()[i].trade_time)
                return i;
        }
        return -1;
    }
    //double Hn(int data_i, int n) const {
    //    assert(psticks() != nullptr);
    //    double max = psticks()[data_i].high;
    //    const Candlestick* begin;
    //    const Candlestick* end;
    //    begin = psticks() + data_i - n + 1;
    //    end = psticks() + data_i;
    //    // int begin = data_i - n + 1;
    //    begin = MAX(psticks(), begin);
    //    for (const Candlestick* p = begin; p <= end; p++ ) {
    //        if (p->high > max) {
    //            max = p->high;
    //        }
    //    }
    //    return max;
    //}
    double Hn(int data_i, int n) const {
        assert(this->sticks().size() > 1);
        double max = this->sticks()[data_i].high;
        int begin = data_i - n + 1;
        begin = MAX(0, begin);
        for (int i = begin; i <= data_i; i++) {
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
    bool IsHighNewHigh(int data_i, int n) const {
        assert(this->sticks().size() > 1);
        int begin = data_i - n;
        begin = MAX(0, begin);
        for (int i = begin; i < data_i; i++) {
            if (sticks()[data_i].high < sticks()[i].high)
                return false;
        }
        return true;
    }
    bool IsLowNewLow(int data_i, int n) const {
        assert(this->sticks().size() > 1);
        int begin = data_i - n;
        begin = MAX(0, begin);
        for (int i = begin; i < data_i; i++) {
            if (sticks()[data_i].low > sticks()[i].low)
                return false;
        }
        return true;
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
    const KDJ* kdj(int n, int sn1, int sn2) {
        std::vector<std::unique_ptr<KDJ>>::iterator it = std::find_if(kdjs_.begin(), kdjs_.end(), [&](const std::unique_ptr<KDJ> & v) {
            return (v->n == n && v->sn1 == sn1 && v->sn2 == sn2);
        });
        if (it == kdjs_.end()) {
            kdjs_.push_back(std::make_unique<KDJ>(this, n, sn1, sn2, true, true));
            return kdjs_.back().get();
        } else
            return (*it).get();
    }
    const BOLL* boll(int n, int p) {
        std::vector<std::unique_ptr<BOLL>>::iterator it = std::find_if(bolls_.begin(), bolls_.end(), [&](const std::unique_ptr<BOLL> & v) {
            return (v->n == n && v->p == p);
        });
        if (it == bolls_.end()) {
            bolls_.push_back(std::make_unique<BOLL>(this, n, p));
            return bolls_.back().get();
        } else
            return (*it).get();
    }
    const POI* poi(int n) {
        std::vector<std::unique_ptr<POI>>::iterator it = std::find_if(pois_.begin(), pois_.end(), [&](const std::unique_ptr<POI> & v) {
            return (v->n == n);
        });
        if (it == pois_.end()) {
            pois_.push_back(std::make_unique<POI>(this, n));
            return pois_.back().get();
        } else
            return (*it).get();
    }
};