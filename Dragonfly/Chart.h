#pragma once

#include <iostream>
#include <map>
#include <cassert>

#include "Candlestick.h"
#include "MACD.h"
#include "MTI.h"
#define FMT_HEADER_ONLY
#include "fmt\format.h"

class Chart {
public:
    enum class Period {
        Minutes5,
        Minutes15,
        Minutes30,
        Minutes60,
        Day,
        End
    };
protected:
    Period period_;
    std::string id_;
    std::string name_;
    std::vector<Candlestick> sticks_;
    //Candlestick* psticks_ = nullptr;
    std::map<std::string, std::vector<MACD>> macds_;
    std::vector<std::unique_ptr<BOLL>> bolls_;
    std::vector<std::unique_ptr<POI>> pois_;
    std::vector<std::unique_ptr<MTI<2>>> mti2_;
public:
    std::vector<std::unique_ptr<KDJ>> kdjs_;
public:
    Period period() const {
        return period_;
    }
    const std::string& id() const {
        return id_;
    }
    const std::string& name() const {
        return name_;
    }
    // std::vector<std::vector<MACD>*> qmacds;
    //std::vector<std::unique_ptr<KDJ>> qkdjs;
public:
    //std::map<int, std::vector<double>> rsvs;
    Chart() {}
    Chart(const std::vector<Candlestick>& src, Period period, const std::string& id, const std::string& name, bool is_init_mit2 = true): period_(period), id_(id), name_(name) {
        std::copy(src.begin(), src.end(), back_inserter(sticks_));
        InitSticks();
        if (is_init_mit2) {
            InitMTI2();
        }
    }
    void InitSticks() {
        sticks_[0].prev = nullptr;
        for (int i = 1; i < sticks_.size(); i++) {
            sticks_[i].prev = &sticks_[i - 1];
        }
    }
    void InitMTI2() {
        assert(sticks().size() > 0);
        if (mti2_.size() == 0) {
            std::unique_ptr<MTI<2>> newMTI2(new MTI<2>(this->sticks(), *kdj(9, 3, 3)));
            this->mti2_.push_back(std::move(newMTI2));
        }
    }
    //void LoadData
    //Chart(const std::vector<Candlestick>&& src) {
    //    //std::cout << "Chart move\n";
    //    sticks_ = src;
    //    if (sticks_.size() == 0)
    //        return;
    //    sticks_[0].prev = nullptr;
    //    for (int i = 1; i < sticks_.size(); i++) {
    //        sticks_[i].prev = &sticks_[i - 1];
    //    }
    //}
    ~Chart() {
        //DELETE_IF_NOT_NULLPTR(psticks_);
    }
    const std::vector<Candlestick>& sticks() const {
        return sticks_;
    }
    //const Candlestick* psticks() const {
    //    return psticks_;
    // }
    MTI<2>* mti2() {
        return mti2_[0].get();
    }
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
                // assert(0 > 1);
                rsv = 0.0;
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