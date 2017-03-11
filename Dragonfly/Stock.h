#pragma once

#include "Trade.h"
#include "util.h"
#include "sqlite/sqlite3.h"

#pragma warning(disable: 4244) // warning C4244: conversion from 'double' to 'float', possible loss of data
#pragma warning(disable: 4996) // Error	C4996	'sprintf': This function or variable may be unsafe.Consider using sprintf_s instead.To disable deprecation, use _CRT_SECURE_NO_WARNINGS.See online help for details.

class Stock : public Chart {
public:
    Stock(const std::string& id, const std::string& name, DateTime beginDate, DateTime endDate) {
        id_ = id;
        name_ = name;
        std::string path = "D:\\Data Center\\bourgogne\\hdata\\" + this->id() + ".db";
        if (!FileExists(path))
            return;
        LoadData(path, beginDate, endDate);
    }
private:
    void LoadData(const std::string& fullfilename, DateTime beginDate, DateTime endDate) {
        if (!FileExists(fullfilename)) {
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
        // char str[128];
        std::string str = fmt::format("SELECT date,open,close,high,low,volume FROM tick_data where {0} <= date AND date <= {1} order by date", beginDate.ToString("%Y%m%d"), endDate.ToString("%Y%m%d"));
        //sprintf(str, "SELECT date,open,close,high,low,volume FROM tick_data where %s <= date AND date <= %s order by date", beginDate.time.c_str(), endDate.time.c_str());
        //std::cout << str << "\n";
        const char *zSql = str.c_str();
        r = sqlite3_prepare_v2(db, zSql, -1, &stmt, &zTail);
        if (r != SQLITE_OK) {
            fprintf(stderr, "%s:%s %s\n", "sqlite3_prepare_v2", this->id().c_str(), sqlite3_errmsg(db));
            return;
        }
        std::vector<Candlestick> sticks;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Candlestick d;
            int int_date = sqlite3_column_int(stmt, 0);
            std::string strDate = std::to_string(int_date);
            strDate.insert(4, "-");
            strDate.insert(7, "-");
            // std::string strDate = p;// (p, strlen(p)); // = p;
            d.trade_time = strDate;
            //int column0 = sqlite3_column_int(stmt, 0);
            //eight_digit_time digit_time = eight_digit_time(column0);
            //d.begin_time = digit_time;
            d.open = sqlite3_column_double(stmt, 1);
            d.close = sqlite3_column_double(stmt, 2);
            d.high = sqlite3_column_double(stmt, 3);
            d.low = sqlite3_column_double(stmt, 4);
            d.volume = sqlite3_column_double(stmt, 5);

            sticks.push_back(d);
        }
        this->sticks_ = std::move(sticks);
        InitSticks();
        InitMTI2();
        // std::unique_ptr<Chart> chart(new Chart(std::move(sticks)));
        // this->charts_.resize((int)TradeDataPeriod::End);
        // this->charts_[(int)TradeDataPeriod::Day] = std::move(chart);
        //this->UpdateCharts();
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
};
//class Stock : public Trade {
//public:
//    //class Indicators {
//    //pub
//    //    std::vector<float> asi_;
//    /
//
//protected:
//    std::string id_;
//    std::string name_;
//    float pe_;
//public: // property
//    std::string industry_;
//    const Stock* index399001_;
//    const std::string& industry() const {
//        return industry_;
//    }
//    const std::string& id() const {
//        return id_;
//    }
//    const std::string& name() const {
//        return name_;
//    }
//    const double pe() const {
//        return pe_;
//    }
//    const std::vector<SakataValuesPart1>& sakata_values_part1() const {
//        return sakata_values_part1_;
//    }
//    const std::vector<SakataValuesPart2>& sakata_values_part2() const {
//        return sakata_values_part2_;
//    }
//    const Stock* index399001() const {
//        return this->index399001_;
//    }
//    // use for fast access
//    // please init it first by call InitTradeDataBlock
//    const std::vector<const TradeData*>& trade_data_block() const {
//        return trade_data_block_;
//    }
//public:
//   // const Indicators& indicators() const {
//   //     return indicators_;
//   // }
//private:
//    Stock() = delete;
//    Stock(const Stock& s) = delete;
//    Stock(Stock&& s) = delete;
//    Stock& operator =(const Stock& s) = delete;
//public:
//    Stock(const std::string & id, const std::string & name, TradeDataType dt, const eight_digit_time& beginDate, const eight_digit_time& endDate = 33333333);
//    Stock(const std::string & id, const std::string & name, double pe, TradeDataType dt, const eight_digit_time & beginDate, const eight_digit_time& endDate = 33333333) : Stock(id, name, dt, beginDate, endDate) {
//        this->pe_ = pe;
//    }
//    ~Stock();
//public:
//    void InitTradeDataBlock(const Stock& index) {
//        trade_data_block_.clear();
//        trade_data_block_.reserve(index.trade_data().size());
//        for (size_t i = 0; i < trade_data_block_.size(); i++) {
//            trade_data_block_[i] = nullptr;
//        }
//        for (size_t i = 0; i < index.trade_data().size(); i++) {
//            //int index_i = index.GetIndex((index.trade_data()[i].begin_time));
//            int this_i = this->GetIndex(index.trade_data()[i].begin_time);
//            if (this_i != -1) {
//                const TradeData* p = &trade_data()[this_i];
//                trade_data_block_.push_back(p);
//            } else {
//                trade_data_block_.push_back(nullptr);
//            }
//        }
//        assert(trade_data_block_.size() == index.trade_data().size());
//    }
//    void UpdateIndicators() {
//        indicators_.Update();
//
//        // update sakata values
//        if (this->trade_data().size() < 10)
//            return;
//
//        sakata_values_part1_.clear();
//        sakata_values_part1_.resize(this->trade_data_.size());
//
//        assert(sizeof(sakata_values_part1_[0].bits) == 4);
//        sakata_values_part1_[0].bits = 0xffff; // this values is invalid
//        sakata_values_part1_[1].bits = 0xffff;
//        for (size_t i = 2; i < this->trade_data_.size(); i++) {
//            sakata_values_part1_[i].candle_type_today = this->trade_data()[i].candle_type();
//            sakata_values_part1_[i].candle_type_yesterday = this->trade_data()[i - 1].candle_type();
//            sakata_values_part1_[i].candle_type_before_yesterday = this->trade_data()[i - 2].candle_type();
//            sakata_values_part1_[i].today_close_position_type = this->trade_data()[i].close_type();
//        }
//        assert(sakata_values_part1_.size() >= 5);
//
//        sakata_values_part2_.clear();
//        sakata_values_part2_.resize(this->trade_data_.size());
//
//        assert(sizeof(sakata_values_part2_[0].bits) == 4);
//        sakata_values_part2_[0].bits = 0xffff; // this values is invalid
//        sakata_values_part2_[1].bits = 0xffff;
//        for (size_t i = 2; i < this->trade_data_.size(); i++) {
//            sakata_values_part2_[i].today_is_high_new_high_20days = this->trade_data()[i].is_high_new_high(20);
//            sakata_values_part2_[i].today_is_low_new_low_20days = this->trade_data()[i].is_low_new_low(20);
//            sakata_values_part2_[i].yesterday_is_high_new_high_20days = this->trade_data()[i - 1].is_high_new_high(20);
//            sakata_values_part2_[i].yesterday_is_low_new_low_20days = this->trade_data()[i - 1].is_low_new_low(20);
//            sakata_values_part2_[i].before_yesterday_is_high_new_high_20days = this->trade_data()[i - 2].is_high_new_high(20);
//            sakata_values_part2_[i].before_yesterday_is_low_new_low_20days = this->trade_data()[i - 2].is_low_new_low(20);
//            sakata_values_part2_[i].today_is_high_new_high_5days = this->trade_data()[i].is_high_new_high(5);
//            sakata_values_part2_[i].today_is_low_new_low_5days = this->trade_data()[i].is_low_new_low(5);
//            sakata_values_part2_[i].yesterday_is_high_new_high_5days = this->trade_data()[i - 1].is_high_new_high(5);
//            sakata_values_part2_[i].yesterday_is_low_new_low_5days = this->trade_data()[i - 1].is_low_new_low(5);
//            sakata_values_part2_[i].before_yesterday_is_high_new_high_5days = this->trade_data()[i - 2].is_high_new_high(5);
//            sakata_values_part2_[i].before_yesterday_is_low_new_low_5days = this->trade_data()[i - 2].is_low_new_low(5);
//            sakata_values_part2_[i].today_is_high_new_high_60days = this->trade_data()[i].is_high_new_high(60);
//            sakata_values_part2_[i].today_is_low_new_low_60days = this->trade_data()[i].is_low_new_low(60);
//            sakata_values_part2_[i].yesterday_is_high_new_high_60days = this->trade_data()[i - 1].is_high_new_high(60);
//            sakata_values_part2_[i].yesterday_is_low_new_low_60days = this->trade_data()[i - 1].is_low_new_low(60);
//            sakata_values_part2_[i].before_yesterday_is_high_new_high_60days = this->trade_data()[i - 2].is_high_new_high(60);
//            sakata_values_part2_[i].before_yesterday_is_low_new_low_60days = this->trade_data()[i - 2].is_low_new_low(60);
//            /*sakata_values_part2_[i].today_is_stick_up                       = this->trade_data()[i].is_stick_up();
//            sakata_values_part2_[i].today_is_j_gt_k                         = this->indicators().kdj()[i].j > this->indicators().kdj()[i].k;
//            sakata_values_part2_[i].today_is_high_new_high_5days            = this->trade_data()[i].is_high_new_high(5);
//            sakata_values_part2_[i].today_is_low_new_low_5days              = this->trade_data()[i].is_low_new_low(5);
//            sakata_values_part2_[i].today_is_ma20_up                        = this->indicators().ma20()[i] > this->indicators().ma20()[i - 1];
//            sakata_values_part2_[i].today_is_ma60_up                        = this->indicators().ma60()[i] > this->indicators().ma60()[i - 1];
//            sakata_values_part2_[i].yesterday_is_high_new_high_5days        = this->trade_data()[i - 1].is_high_new_high(5);
//            sakata_values_part2_[i].yesterday_is_low_new_low_5days          = this->trade_data()[i - 1].is_low_new_low(5);
//            sakata_values_part2_[i].before_yesterday_is_high_new_high_5days = this->trade_data()[i - 2].is_high_new_high(5);
//            sakata_values_part2_[i].before_yesterday_is_low_new_low_5days   = this->trade_data()[i - 2].is_low_new_low(5);*/
//        }
//    }
//    void UpdateDataLink() {
//        if (trade_data().empty())
//            return;
//        // if not empty, init the link
//        trade_data_[0].prev = nullptr;
//        for (size_t i = 1; i < this->trade_data_.size(); i++) {
//            trade_data_[i].prev = &trade_data_[i - 1];
//            trade_data_[i].Update();
//        }
//        for (size_t i = 0; i < this->trade_data_.size(); i++) {
//            trade_data_[i].i = i;
//        }
//        if (this->name()[0] == 'S' || this->name()[0] == '*') {
//            for (size_t i = 0; i < this->trade_data_.size(); i++) {
//                trade_data_[i].is_S_ST_StarST = true;
//            }
//        } else {
//            for (size_t i = 0; i < this->trade_data_.size(); i++) {
//                trade_data_[i].is_S_ST_StarST = false;
//            }
//        }
//    }
//private:
//    void LoadData(const eight_digit_time& beginDate, const eight_digit_time& endDate, const std::string& fullfilename) {
//        if(!FileExists(fullfilename)) {
//            return;
//        }
//        sqlite3 *db;
//        sqlite3_stmt *stmt;
//        const char *zTail;
//
//        int r = sqlite3_open(fullfilename.c_str(), &db);
//        if (r) {
//            printf("%s", sqlite3_errmsg(db));
//            fprintf(stderr, "%s: %s\n", "open", sqlite3_errmsg(db));
//            return;
//        }
//        char str[128];
//        sprintf(str, "SELECT * FROM tick_data where %d <= date AND date <= %d order by date", beginDate.value(), endDate.value());
//        const char *zSql = str;
//        r = sqlite3_prepare_v2(db, zSql, -1, &stmt, &zTail);
//        if (r != SQLITE_OK) {
//            fprintf(stderr, "%s:%s %s\n", "sqlite3_prepare_v2", this->id().c_str(), sqlite3_errmsg(db));
//            return;
//        }
//        while (sqlite3_step(stmt) == SQLITE_ROW) {
//            TradeData d;
//            int column0 = sqlite3_column_int(stmt, 0);
//            eight_digit_time digit_time = eight_digit_time(column0);
//            d.begin_time = digit_time;
//            d.open = sqlite3_column_double(stmt, 1);
//            d.high = sqlite3_column_double(stmt, 2);
//            d.low = sqlite3_column_double(stmt, 3);
//            d.close = sqlite3_column_double(stmt, 4);
//            d.volume = sqlite3_column_double(stmt, 5);
//            this->trade_data_.push_back(d);
//        }
//        sqlite3_finalize(stmt);
//        sqlite3_close(db);
//
//        UpdateDataLink();
//    }
//private:
//    void LoadData(const eight_digit_time& beginDate, const eight_digit_time& endDate) {
//        std::string fullfilename;
//        if (this->id()[0] == 'i') {
//            fullfilename = "D:\\Data Center\\bourgogne\\index\\";//000001.db";
//            std::string str = id();
//            fullfilename += str.erase(0, 5);
//            fullfilename += ".db";
//        } else if (this->id().length() == 6 && std::all_of(this->id().begin(), this->id().end(), ::isdigit)) {
//            fullfilename = "D:\\Data Center\\bourgogne\\hdata\\" + id() + ".db";
//        } else {
//            fullfilename = "D:\\Data Center\\bourgogne\\hdata_futures\\" + id() + ".db";
//        }
//        return LoadData(beginDate, endDate, fullfilename);
//    }
//public:
//    double Hn(year_month_day_hour_min_sec time, int n) const {
//        int i = GetIndex(time);
//        return Hn(i, n);
//    }
//    // highest in n days
//    double Hn(int data_i, int n) const {
//        assert(this->trade_data().size() > 1);
//        double max = this->trade_data()[data_i].high;
//        int begin = data_i - n + 1;
//        begin = MAX(0, begin);
//        for (int i = begin; i <= data_i; i++ ) {
//            if (this->trade_data()[i].high > max) {
//                max = this->trade_data()[i].high;
//            }
//        }
//        return max;
//    }
//    year_month_day_hour_min_sec GetHighestDate(int begin_index, int end_index) const {
//        int hi = begin_index;
//        for (int i = begin_index + 1; i <= end_index; i++) {
//            if (this->trade_data()[hi].high < this->trade_data()[i].high) {
//                hi = i;
//            }
//        }
//        return this->trade_data()[hi].begin_time;
//    }
//    /* int GetIndex(const year_month_day_hour_min_sec& time) const {
//         int i = 0;
//         for (auto && d : this->trade_data()) {
//             if (d.begin_time == time)
//                 return i;
//             ++i;
//         }
//         return -1;
//     }*/
//    int GetIndex(const year_month_day_hour_min_sec& time) const {
//        for (size_t i = 0; i < this->trade_data().size(); i++) {
//            if (this->trade_data()[i].begin_time == time)
//                return i;
//        }
//        return -1;
//    }
//    bool is_suspended(const year_month_day_hour_min_sec& time, const Stock& index) const {
//        if (index.GetLatestTradingTime(time).epoch() != this->GetLatestTradingTime(time).epoch()) {
//            return true;
//        }
//        return false;
//    }
//    bool IsTradingDay(const year_month_day_hour_min_sec& time) const {
//        return GetIndex(time) != -1;
//    }
//    year_month_day_hour_min_sec GetLatestTradingTime(const year_month_day_hour_min_sec& time) const {
//        year_month_day_hour_min_sec ret = time;
//        for (int i = 0; i < 30; i++) {
//            if (GetIndex(ret) != -1) {
//                //std::cout << "GetIndex!=-1" << std::endl;
//                return ret;
//            }
//            ret.AddDay(-1);
//        }
//        return year_month_day_hour_min_sec(0, 0, 0);
//    }
//    year_month_day_hour_min_sec GetDate(int i) const {
//        return trade_data()[i].begin_time;
//    }
//    double Ln(year_month_day_hour_min_sec date, int n) const {
//        int i = GetIndex(date);
//        return Ln(i, n);
//    }
//    // lowest in n days
//    double Ln(int i, int n) const {
//        double min = this->trade_data()[i].low;
//        int end = i - n;
//        i--;
//        for (; i > end && i >= 0; i--) {
//            if (this->trade_data()[i].low < min) {
//                min = this->trade_data()[i].low;
//            }
//        }
//        return min;
//    }
//
//    double HighestFallPercent(int nDays, int pastn) const {
//        return 1 - this->Ln(trade_data().size() - 1, pastn) / this->Hn(trade_data().size() - 1, nDays);
//    }
//};
////inline bool compareBy(const Stock &a, const Stock &b) {
////    return a.id < b.id;
////}
