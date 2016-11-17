#pragma once

#include <sys/stat.h>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <string>

#include <boost/system/error_code.hpp>

#include "csv.h"
#include "sqlite/sqlite3.h"
#include "util.h"

#include "KDJ.h"
#include "ARBR.h"
#include "BOLL.h"
#include "DMI.h"
#include "VR.h"

class ASI;
class MFI;
class MTI2;
class MTI3;
class FMTI2;
class FMTI3;

#pragma warning(disable: 4244) // warning C4244: conversion from 'double' to 'float', possible loss of data
#pragma warning(disable: 4996) // Error	C4996	'sprintf': This function or variable may be unsafe.Consider using sprintf_s instead.To disable deprecation, use _CRT_SECURE_NO_WARNINGS.See online help for details.


#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN3(a,b) MIN(MIN(a,b),c)
#define MAX3(a,b,c) MAX(MAX(a,b),c)

using namespace boost::system;

//class HData {
//public:
//    std::time_t begin_time;
//    int date; // like 20160130
//    double open, close, high, low, volume;
//public:
//};
#define TRADA_DATA_FLOAT float
struct TradeData {
    enum CandleType {
        LittleHollow, // amp < 1%
        LittleFilled,
        Hollow_NoShadow, // reverse direction less than 10% of the body
        Hollow_XShortUpperShadow, // 10%-25% of the body
        Hollow_ShortUpperShadow, // 25%-50% of the body
        Hollow_MediumUpperShadow, // 50%-100%
        Hollow_LongUpperShadow, // 100%-200%
        Hollow_XLongUpperShadow, // above 200%
        Filled_NoShadow, // reverse direction less than 10% of the body
        Filled_XShortLowerShadow, // 10%-25% of the body
        Filled_ShortLowerShadow, // 25%-50% of the body
        Filled_MediumLowerShadow, // 50%-100%
        Filled_LongLowerShadow, // 100%-200%
        Filled_XLongLowerShadow, // above 200%
        End
    };
    enum CandleTypeClass2 {
        RealLittleHollow2, // amp < 1%
        FakeLittleHollow2,
        RealLittleFilled2,
        FakeLittleFilled2,
        RealHollow,
        FakeHollow,
        RealFilled,
        FakeFilled,
        End2
    };
    TradeData* prev;
    int i; // index self
    year_month_day_hour_min_sec begin_time;
    TRADA_DATA_FLOAT open, close, high, low, volume;
    bool is_S_ST_StarST;
private:
    bool is_stick_up_;
    bool is_stick_filled_;
    TRADA_DATA_FLOAT body_up_line_;
    TRADA_DATA_FLOAT upper_shadow_amplitude_;
    TRADA_DATA_FLOAT lower_shadow_amplitude_;
    TRADA_DATA_FLOAT amplitude_;
    TRADA_DATA_FLOAT body_amplitude_;
    CandleType candle_type_;
    CandleTypeClass2 candle_type_class2_;
    bool can_open_long_position_at_close_price_;
public: // property
    bool is_stick_up() const {
        return is_stick_up_;
    }
    bool is_stick_filled() const {
        return is_stick_filled_;
    }
    TRADA_DATA_FLOAT body_up_line() const {
        return body_up_line_;
    }
    TRADA_DATA_FLOAT upper_shadow_amplitude() const {
        return upper_shadow_amplitude_;
    }
    TRADA_DATA_FLOAT lower_shadow_amplitude() const {
        return lower_shadow_amplitude_;
    }
    TRADA_DATA_FLOAT amplitude() const {
        return amplitude_;
    }
    TRADA_DATA_FLOAT body_amplitude() const {
        return body_amplitude_;
    }
    bool has_long_tail() const {
        if (body_amplitude() < 1.0 / 100) {
            if (is_stick_up()) {
                return lower_shadow_amplitude() > 2.0 / 100;
            } else
                return upper_shadow_amplitude() > 2.0 / 100;
        } else {
            if (is_stick_up()) {
                return lower_shadow_amplitude() > body_amplitude();
            } else
                return upper_shadow_amplitude() > body_amplitude();
        }
    }
    CandleType candle_type() const {
        return candle_type_;
    }
    CandleTypeClass2 candle_type_class2() const {
        return candle_type_class2_;
    }
    bool can_open_long_position_at_close_price() const {
        return can_open_long_position_at_close_price_;
    }
public:
    void Update() {
        is_stick_up_ = close > open;
        is_stick_filled_ = close < open;
        body_up_line_ = MAX(open, close);
        upper_shadow_amplitude_ = (high - body_up_line()) / open;
        lower_shadow_amplitude_ = (MIN(open, close) - low) / open;
        amplitude_ =  abs(high - low) / open;
        body_amplitude_ = abs(open - close) / open;
        UpdateCandleType();
        UpdateCandleTypeClass2();
        UpdateCanOpenLongPositionAtClosePrice();
    }
private:
    void UpdateCandleType() {
        if (body_amplitude() < 1.0 / 100) {
            if (is_stick_up()) {
                candle_type_ = LittleHollow;
                return;
            } else {
                candle_type_ = LittleFilled;
                return;
            }
        } else {
            if (is_stick_up()) {
                if (upper_shadow_amplitude() < body_amplitude() * 10.0 / 100) {
                    candle_type_ = Hollow_NoShadow;
                    return;
                }
                if (upper_shadow_amplitude() < body_amplitude() * 25.0 / 100) {
                    candle_type_ = Hollow_XShortUpperShadow;
                    return;
                }
                if (upper_shadow_amplitude() < body_amplitude() * 50.0 / 100) {
                    candle_type_ = Hollow_ShortUpperShadow;
                    return;
                }
                if (upper_shadow_amplitude() < body_amplitude() * 100.0 / 100) {
                    candle_type_ = Hollow_MediumUpperShadow;
                    return;
                }
                if (upper_shadow_amplitude() < body_amplitude() * 200.0 / 100) {
                    candle_type_ = Hollow_LongUpperShadow;
                    return;
                } else  {
                    candle_type_ = Hollow_XLongUpperShadow;
                    return;
                }
            } else { // stick filled
                if (lower_shadow_amplitude() < body_amplitude() * 10.0 / 100) {
                    candle_type_ = Filled_NoShadow;
                    return;
                }
                if (lower_shadow_amplitude() < body_amplitude() * 25.0 / 100) {
                    candle_type_ = Filled_XShortLowerShadow;
                    return;
                }
                if (lower_shadow_amplitude() < body_amplitude() * 50.0 / 100) {
                    candle_type_ = Filled_ShortLowerShadow;
                    return;
                }
                if (lower_shadow_amplitude() < body_amplitude() * 100.0 / 100) {
                    candle_type_ = Filled_MediumLowerShadow;
                    return;
                }
                if (lower_shadow_amplitude() < body_amplitude() * 200.0 / 100) {
                    candle_type_ = Filled_LongLowerShadow;
                    return;
                } else {
                    candle_type_ = Filled_XLongLowerShadow;
                    return;
                }
            }
        }
    }
    void UpdateCandleTypeClass2() {
        if (body_amplitude() < 1.0 / 100) {
            if (is_stick_up()) {
                if (is_price_up()) {
                    candle_type_class2_ = CandleTypeClass2::RealLittleHollow2;
                    return;
                } else {
                    candle_type_class2_ = CandleTypeClass2::FakeLittleHollow2;
                    return;
                }
            } else {
                if(is_price_down()) {
                    candle_type_class2_ = CandleTypeClass2::RealLittleFilled2;
                    return;
                } else {
                    candle_type_class2_ = CandleTypeClass2::FakeLittleFilled2;
                    return;
                }

            }
        } else {
            if (is_stick_up()) {
                if (is_price_up()) {
                    candle_type_class2_ = CandleTypeClass2::RealHollow;
                    return;
                } else {
                    candle_type_class2_ = CandleTypeClass2::FakeHollow;
                    return;
                }
            } else {
                if (is_price_down()) {
                    candle_type_class2_ = CandleTypeClass2::RealFilled;
                    return;
                } else {
                    candle_type_class2_ = CandleTypeClass2::FakeFilled;
                    return;
                }
            }
        }
    }
    void UpdateCanOpenLongPositionAtClosePrice() {
        if (abs(high - low) < 0.001) {
            can_open_long_position_at_close_price_ = false;
            return;
        }
        if (up_percent() > 9.85 / 100) {
            can_open_long_position_at_close_price_ = false;
            return;
        }
        if (is_S_ST_StarST && up_percent() > 4.85 / 100) {
            can_open_long_position_at_close_price_ = false;
            return;
        }
        can_open_long_position_at_close_price_ = true;
    }
public:
    TradeData() {
    }
    bool is_close_new_high(int n) const {
        TradeData* past = this->prev;
        for (int i = 0; i < n && (past != nullptr); i++, past = past->prev) {
            if (close < past->high)
                return false;
        }
        return true;
    }
    bool is_open_higher_than_prev_high() const {
        return open > prev->high;
    }
    bool is_body_contains(float price) {
        return (MIN(open, close) <= price && price <= MAX(open, close));
    }
    float high_percent() const {
        return (high - prev->close) / prev->close;
    }
    float up_percent() const {
        return (close - prev->close) / prev->close;
    }
    float low_percent() const {
        return (low - prev->close) / prev->close;
    }
    float open_percent() const {
        return (open - prev->close) / prev->close;
    }
    float body_middle() const {
        return (open + close) / 2;
    }
    void set_begin_time(year_month_day_hour_min_sec time) {
        this->begin_time = time;
    }
    double percent_change() const {
        //assert(prev != nullptr);
        if (prev == nullptr)
            return 0.0;
        return (close - prev->close) / prev->close;
    }
    bool has_upper_shadow() const {
        if (high > open && high > close)
            return true;
        else
            return false;
    }
    bool has_lower_shadow() const {
        if (low < open && low < close)
            return true;
        else
            return false;
    }
    double percent_change_today() const {
        return (close - open) / open;
    }
    bool is_price_up() const {
        return (close - prev->close) > 0;
    }
    bool is_price_up(int n) const {
        // 0 today
        // 1 yestday
        const TradeData* day_before = this;
        for (int i = 0; i < n; i++) {
            day_before = day_before->prev;
        }
        return (close - day_before->close) > 0;
    }
    bool is_price_down(int n) const {
        // 0 today
        // 1 yestday
        const TradeData* day_before = this;
        for (int i = 0; i < n; i++) {
            day_before = day_before->prev;
        }
        return (close - day_before->close) < 0;
    }
    bool is_price_down() const {
        return (close - prev->close) < 0;
    }
    bool is_flat_open(double delta) const {
        assert(delta < 0.1);
        if (abs(open - prev->close) < (delta * prev->close)) {
            return true;
        } else {
            return false;
        }

    }
    bool is_volume_increase(double percent = 0.1) const {
        return volume > prev->volume * (1 + percent);
    }
    bool is_volume_decrease(double percent = 0.1) const {
        return !is_volume_increase(percent);
    }
};
enum class TradeDataType {
    Day,
    OneMinute,
    FiveMinutes,
    TenMinutes
};
class Stock {
public:
    class Indicators {
    public:
        const Stock* stock;
        std::vector<float> rsi5_;
        std::vector<float> rsi10_;
        std::vector<KDJ> kdj_;
        /*std::vector<MTI2> mti2_;
        std::vector<MTI3> mti3_;
        std::vector<FMTI2> fmti2_;
        std::vector<FMTI3> fmti3_;*/
        std::vector<BOLL> boll_;
        std::vector<DMI_I> dmi_;
        std::vector<VR> vr_;
        std::vector<float> asi_;
        std::vector<ARBR> arbr_;
        //std::vector<float> ma5_;
        //std::vector<float> ma10_;
        //std::vector<float> ma20_;
        //std::vector<float> ma60_;
        //Indicators(const Stock& owner) {
        //}
        const std::vector<float>& rsi5() const {
            return rsi5_;
        }
        const std::vector<float>& rsi10() const {
            return rsi10_;
        }
        const std::vector<ARBR>& arbr() const {
            return arbr_;
        }
        const std::vector<KDJ>& kdj() const {
            return kdj_;
        }
        /*const std::vector<MTI2>& mti2() const {
            return mti2_;
        }
        const std::vector<MTI3>& mti3() const {
            return mti3_;
        }
        const std::vector<FMTI2>& fmti2() const {
            return fmti2_;
        }
        const std::vector<FMTI3>& fmti3() const {
            return fmti3_;
        }*/
        const std::vector<DMI_I>& dmi() const {
            return dmi_;
        }
        const std::vector<VR>& vr() const {
            return vr_;
        }
        const std::vector<BOLL>& boll() const {
            return boll_;
        }
        const std::vector<float>& asi() const {
            return asi_;
        }
        /*  const std::vector<float>& ma5() const {
              return ma5_;
          }
          const std::vector<float>& ma10() const {
              return ma10_;
          }
          const std::vector<float>& ma20() const {
              return ma20_;
          }
          const std::vector<float>& ma60() const {
              return ma60_;
          }*/
        void InitKDJ(int n);
        // void InitMA();
        void InitDMI(int n1, int n2);
        void Update();
    };
    Indicators indicators_;

public:
    //enum class Type {
    //    Normal,
    //    Futures,
    //    Index
    //};
protected:
    std::string id_;
    std::string name_;
    float pe_;
    TradeDataType trade_data_type_;
    std::vector<TradeData> trade_data_;
    std::vector<const TradeData*> trade_data_block_;
public: // property
    const std::string& id() const {
        return id_;
    }
    const std::string& name() const {
        return name_;
    }
    const double pe() const {
        return pe_;
    }
    const TradeDataType trade_data_type() const {
        return trade_data_type_;
    }
    const std::vector<TradeData>& trade_data() const {
        return trade_data_;
    }
    // use for fast access
    // please init it first by call InitTradeDataBlock
    const std::vector<const TradeData*>& trade_data_block() const {
        return trade_data_block_;
    }
public:
    const Indicators& indicators() const {
        return indicators_;
    }
private:
    Stock() = delete;
    Stock(const Stock& s) = delete;
    Stock(Stock&& s) = delete;
    Stock& operator =(const Stock& s) = delete;
public:
    Stock(const std::string & id, const std::string & name, TradeDataType dt, const eight_digit_time& beginDate);
    Stock(const std::string & id, const std::string & name, double pe, TradeDataType dt, const eight_digit_time & beginDate) : Stock(id, name, dt, beginDate) {
        this->pe_ = pe;
    }
    ~Stock();
public:
    void InitTradeDataBlock(const Stock& index) {
        trade_data_block_.clear();
        trade_data_block_.reserve(index.trade_data().size());
        for (size_t i = 0; i < trade_data_block_.size(); i++) {
            trade_data_block_[i] = nullptr;
        }
        for (size_t i = 0; i < index.trade_data().size(); i++) {
            //int index_i = index.GetIndex((index.trade_data()[i].begin_time));
            int this_i = this->GetIndex(index.trade_data()[i].begin_time);
            if (this_i != -1) {
                const TradeData* p = &trade_data()[this_i];
                trade_data_block_.push_back(p);
            } else {
                trade_data_block_.push_back(nullptr);
            }
        }
        assert(trade_data_block_.size() == index.trade_data().size());
    }
    void UpdateIndicators() {
        indicators_.Update();
    }
    void UpdateDataLink() {
        if (trade_data().empty())
            return;
        // if not empty, init the link
        trade_data_[0].prev = nullptr;
        for (size_t i = 1; i < this->trade_data_.size(); i++) {
            trade_data_[i].prev = &trade_data_[i - 1];
            trade_data_[i].Update();
        }
        for (size_t i = 0; i < this->trade_data_.size(); i++) {
            trade_data_[i].i = i;
        }
        if (this->name()[0] == 'S' || this->name()[0] == '*') {
            for (size_t i = 0; i < this->trade_data_.size(); i++) {
                trade_data_[i].is_S_ST_StarST = true;
            }
        } else {
            for (size_t i = 0; i < this->trade_data_.size(); i++) {
                trade_data_[i].is_S_ST_StarST = false;
            }
        }
    }
private:
    void LoadData(const eight_digit_time& beginDate, const std::string& fullfilename) {
        if(!FileExists(fullfilename)) {
            return;
        }
        sqlite3 *db;
        sqlite3_stmt *stmt;
        const char *zTail;

        int r = sqlite3_open(fullfilename.c_str(), &db);
        if (r) {
            printf("%s", sqlite3_errmsg(db));
            fprintf(stderr, "%s: %s\n", "open", sqlite3_errmsg(db));
            return;
        }
        char str[128];
        sprintf(str, "SELECT * FROM tick_data where date > %d order by date", beginDate.value());
        const char *zSql = str;
        r = sqlite3_prepare_v2(db, zSql, -1, &stmt, &zTail);
        if (r != SQLITE_OK) {
            fprintf(stderr, "%s:%s %s\n", "sqlite3_prepare_v2", this->id().c_str(), sqlite3_errmsg(db));
            return;
        }
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            TradeData d;
            int column0 = sqlite3_column_int(stmt, 0);
            eight_digit_time digit_time = eight_digit_time(column0);
            d.begin_time = digit_time;
            d.open = sqlite3_column_double(stmt, 1);
            d.high = sqlite3_column_double(stmt, 2);
            d.low = sqlite3_column_double(stmt, 3);
            d.close = sqlite3_column_double(stmt, 4);
            d.volume = sqlite3_column_double(stmt, 5);
            this->trade_data_.push_back(d);
        }
        sqlite3_finalize(stmt);
        sqlite3_close(db);

        UpdateDataLink();
    }
private:
    void LoadData(const eight_digit_time& beginDate) {
        std::string fullfilename;
        if (this->id()[0] == 'i') {
            fullfilename = "D:\\Data Center\\bourgogne\\index\\";//000001.db";
            std::string str = id();
            fullfilename += str.erase(0, 5);
            fullfilename += ".db";
        } else if (this->id().length() == 6 && std::all_of(this->id().begin(), this->id().end(), ::isdigit)) {
            fullfilename = "D:\\Data Center\\bourgogne\\hdata\\" + id() + ".db";
        } else {
            fullfilename = "D:\\Data Center\\bourgogne\\hdata_futures\\" + id() + ".db";
        }
        return LoadData(beginDate, fullfilename);
    }
public:
    double Hn(year_month_day_hour_min_sec time, int n) const {
        int i = GetIndex(time);
        return Hn(i, n);
    }
    // highest in n days
    double Hn(int i, int n) const {
        assert(this->trade_data().size() > 1);
        double max = this->trade_data()[i].high;
        int end = i - n;
        i--;
        for (; i > end && i >= 0; i--) {
            if (this->trade_data()[i].high > max) {
                max = this->trade_data()[i].high;
            }
        }
        return max;
    }
    year_month_day_hour_min_sec GetHighestDate(int begin_index, int end_index) const {
        int hi = begin_index;
        for (int i = begin_index + 1; i <= end_index; i++) {
            if (this->trade_data()[hi].high < this->trade_data()[i].high) {
                hi = i;
            }
        }
        return this->trade_data()[hi].begin_time;
    }
    /* int GetIndex(const year_month_day_hour_min_sec& time) const {
         int i = 0;
         for (auto && d : this->trade_data()) {
             if (d.begin_time == time)
                 return i;
             ++i;
         }
         return -1;
     }*/
    int GetIndex(const year_month_day_hour_min_sec& time) const {
        for (size_t i = 0; i < this->trade_data().size(); i++) {
            if (this->trade_data()[i].begin_time == time)
                return i;
        }
        return -1;
    }
    bool is_suspended(const year_month_day_hour_min_sec& time, const Stock& index) const {
        if (index.GetLatestTradingTime(time).epoch() != this->GetLatestTradingTime(time).epoch()) {
            return true;
        }
        return false;
    }
    bool IsTradingDay(const year_month_day_hour_min_sec& time) const {
        return GetIndex(time) != -1;
    }
    year_month_day_hour_min_sec GetLatestTradingTime(const year_month_day_hour_min_sec& time) const {
        year_month_day_hour_min_sec ret = time;
        for (int i = 0; i < 30; i++) {
            if (GetIndex(ret) != -1) {
                //std::cout << "GetIndex!=-1" << std::endl;
                return ret;
            }
            ret.AddDay(-1);
        }
        return year_month_day_hour_min_sec(0, 0, 0);
    }
    year_month_day_hour_min_sec GetDate(int i) const {
        return trade_data()[i].begin_time;
    }
    double Ln(year_month_day_hour_min_sec date, int n) const {
        int i = GetIndex(date);
        return Ln(i, n);
    }
    // lowest in n days
    double Ln(int i, int n) const {
        double min = this->trade_data()[i].low;
        int end = i - n;
        i--;
        for (; i > end && i >= 0; i--) {
            if (this->trade_data()[i].low < min) {
                min = this->trade_data()[i].low;
            }
        }
        return min;
    }

    double HighestFallPercent(int nDays, int pastn) const {
        return 1 - this->Ln(trade_data().size() - 1, pastn) / this->Hn(trade_data().size() - 1, nDays);
    }
};
//inline bool compareBy(const Stock &a, const Stock &b) {
//    return a.id < b.id;
//}
