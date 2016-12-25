#pragma once

#include <sys/stat.h>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <string>
#include <bitset>

using std::bitset;

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
    enum class SimpleBodyPositionType : uint8_t {
        Above,
        In,
        Below,
        End
    };
    enum class SimpleBodyBodyPositionType : uint8_t {
        AboveAbove,// body up above body bottom above
        AboveIn,
        InIn,
        InBelow,
        BelowBelow,
        Engulfing,
        End
    };
    // enum class SimpleCandleType : uint8_t {
    //     LittleHollow, // amp < 1%
    //     LittleFilled,
    ////     Hollow,
    //    Filled,
    //     End
    //};
    enum class ShadowType : uint8_t {
        NoShadow, // less than 10% of the body  - 0.5
        XShortShadow, // 10%-25% of the body    0.5 - 1
        ShortShadow, // 25%-50% of the body       1-2
        MediumShadow, // 50%-100%        2-3
        LongShadow, // 100%-200%      3-5
        XLongShadow, // above 200%    5-
        End
    };
    enum class SimpleShadowType : uint8_t {
        NoShadow, // less than 10% of the body 0 - 1.0
        MediumShadow, // 10%-100%              1.0 - 3.0
        LongShadow, // 100%-200%               3.0-5.0
        XLongShadow, // above 200%             5.0 -
        End
    };
    enum class CandleType : uint8_t {
        FakeHollow,
        RealHollow,
        FakeFilled,
        RealFilled,
        End
    };
    //static std::string EnumToString(SimpleCandleType em) {
    //    if (em == SimpleCandleType::LittleHollow) {
    //        return "LittleHollow";
    //    }
    //    if (em == SimpleCandleType::LittleFilled) {
    //        return "LittleFilled";
    //    }
    //    if (em == SimpleCandleType::Hollow) {
    //        return "Hollow";
    //    }
    //    if (em == SimpleCandleType::Filled) {
    //        return "Filled";
    //    }
    //    return "End";
    //}
    static std::string EnumToString(ShadowType em) {
        if (em == ShadowType::NoShadow) {
            return "NoShadow";
        }
        if (em == ShadowType::XShortShadow) {
            return "XShortShadow";
        }
        if (em == ShadowType::ShortShadow) {
            return "ShortShadow";
        }
        if (em == ShadowType::MediumShadow) {
            return "MediumShadow";
        }
        if (em == ShadowType::LongShadow) {
            return "LongShadow";
        }
        if (em == ShadowType::XLongShadow) {
            return "XLongShadow";
        }
        return "End";
    }
    static std::string EnumToString(CandleType em) {
        if (em == CandleType::FakeHollow) {
            return "FakeHollow";
        }
        if (em == CandleType::RealHollow) {
            return "RealHollow";
        }
        if (em == CandleType::FakeFilled) {
            return "FakeFilled";
        }
        if (em == CandleType::RealFilled) {
            return "RealFilled";
        }
        return "End";
    }

    TradeData* prev;
    int i; // index self
    year_month_day_hour_min_sec begin_time;
    TRADA_DATA_FLOAT open, close, high, low, volume;
    bool is_S_ST_StarST;
private:
    bool is_stick_up_;
    bool is_stick_filled_;
    bool is_high_new_high_or_is_low_new_low_5days_;
    bool is_high_new_high_5days_;
    bool is_low_new_low_5days_;
    TRADA_DATA_FLOAT body_up_line_;
    TRADA_DATA_FLOAT body_bottom_line_;
    TRADA_DATA_FLOAT upper_shadow_amplitude_;
    TRADA_DATA_FLOAT lower_shadow_amplitude_;
    TRADA_DATA_FLOAT amplitude_;
    TRADA_DATA_FLOAT body_amplitude_;
    //SimpleCandleType simple_candle_type_;
    CandleType candle_type_;
    bool can_open_long_position_at_close_price_;
public: // property
    bool is_stick_up() const {
        return is_stick_up_;
    }
    bool is_stick_filled() const {
        return is_stick_filled_;
    }
    bool is_open_jump() const {
        return abs((open - prev->close) / prev->close) > 1.0 / 100;
    }
    TRADA_DATA_FLOAT body_up_line() const {
        return body_up_line_;
    }
    TRADA_DATA_FLOAT body_bottom_line() const {
        return body_bottom_line_;
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
    TRADA_DATA_FLOAT body_amplitude_with_direction() const {
        return (close - open ) / open;
    }
    TRADA_DATA_FLOAT head_shadow_amplitude() const {
        if (is_stick_up()) {
            return upper_shadow_amplitude();
        } else {
            return lower_shadow_amplitude();
        }
    }
    TRADA_DATA_FLOAT tail_shadow_amplitude() const {
        if (is_stick_up()) {
            return lower_shadow_amplitude();
        } else {
            return upper_shadow_amplitude();
        }
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
    SimpleBodyBodyPositionType body_body_position_type() const {
        if (prev == nullptr)
            return SimpleBodyBodyPositionType::End;
        if (body_up_line() > prev->body_up_line() && body_bottom_line() > prev->body_up_line())
            return SimpleBodyBodyPositionType::AboveAbove;
        if (body_up_line() > prev->body_up_line() && prev->is_price_in_body(body_bottom_line()))
            return SimpleBodyBodyPositionType::AboveIn;
        if (prev->is_price_in_body(body_up_line()) && prev->is_price_in_body(body_bottom_line()))
            return SimpleBodyBodyPositionType::InIn;
        if (prev->is_price_in_body(body_up_line()) && body_bottom_line() < prev->body_bottom_line())
            return SimpleBodyBodyPositionType::InBelow;
        if (body_up_line() < prev->body_bottom_line() && body_bottom_line() < prev->body_bottom_line())
            return SimpleBodyBodyPositionType::BelowBelow;
        if (body_up_line() > prev->body_up_line() && body_bottom_line() < prev->body_bottom_line())
            return SimpleBodyBodyPositionType::Engulfing;
        return SimpleBodyBodyPositionType::End;
    }
    // open close price inclued
    bool is_price_in_body(TRADA_DATA_FLOAT price) const {
        return (body_bottom_line() <= price && price <= body_up_line());
    }
    SimpleBodyPositionType open_type() const {
        if (prev != nullptr) {
            return GetSimpleBodyPositionType(*prev, open);
        } else
            return SimpleBodyPositionType::End;
    }
    SimpleBodyPositionType close_type() const {
        if (prev != nullptr) {
            return GetSimpleBodyPositionType(*prev, close);
        } else
            return SimpleBodyPositionType::End;
    }
    CandleType candle_type() const {
        return candle_type_;
    }
    // SimpleCandleType simple_candle_type() const {
    //    return simple_candle_type_;
    // }
    bool can_open_long_position_at_close_price() const {
        return can_open_long_position_at_close_price_;
    }
    double volume_ratio() const {
        return volume / prev->volume;
    }
public:
    void Update() {
        is_stick_up_ = close > open;
        is_stick_filled_ = close < open;
        body_up_line_ = MAX(open, close);
        body_bottom_line_ = MIN(open, close);
        upper_shadow_amplitude_ = (high - body_up_line()) / open;
        lower_shadow_amplitude_ = (MIN(open, close) - low) / open;
        amplitude_ =  abs(high - low) / open;
        body_amplitude_ = abs(open - close) / open;
        UpdateCandleType();
        //UpdateSimpleCandleType();
        UpdateCanOpenLongPositionAtClosePrice();
        is_high_new_high_or_is_low_new_low_5days_ = is_high_new_high(5) || is_low_new_low(5);
        is_high_new_high_5days_ = is_high_new_high(5);
        is_low_new_low_5days_ = is_low_new_low(5);
    }
private:
    SimpleBodyPositionType GetSimpleBodyPositionType(const TradeData& d, TRADA_DATA_FLOAT price) const {
        if (price - d.body_up_line() > 0.0001)
            return SimpleBodyPositionType::Above;
        else if (price - d.body_bottom_line() < -0.0001) {
            return SimpleBodyPositionType::Below;
        } else {
            return SimpleBodyPositionType::In;
        }
    }
    void UpdateCandleType() {
        if (is_stick_up()) {
            if (is_price_up()) {
                candle_type_ = CandleType::RealHollow;
                return;
            } else {
                candle_type_ = CandleType::FakeHollow;
                return;
            }
        } else {
            if (is_price_down()) {
                candle_type_ = CandleType::RealFilled;
                return;
            } else {
                candle_type_ = CandleType::FakeFilled;
                return;
            }
        }
    }
    //void UpdateSimpleCandleType() {
    //    if (body_amplitude() < 1.0 / 100) {
    //        if (is_stick_up()) {
    //            simple_candle_type_ = SimpleCandleType::LittleHollow;
    //            return;
    //        } else {
    //            simple_candle_type_ = SimpleCandleType::LittleFilled;
    //            return;
    //        }
    //    } else {
    //        if (is_stick_up()) {
    //            simple_candle_type_ = SimpleCandleType::Hollow;
    //            return;
    //        } else { // stick filled
    //            simple_candle_type_ = SimpleCandleType::Filled;
    //            return;
    //        }
    //    }
    //}
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
    bool is_high_new_high(int n) const {
        TradeData* past = this->prev;
        for (int i = 0; i < n && (past != nullptr); i++, past = past->prev) {
            if (high < past->high)
                return false;
        }
        return true;
    }
    bool is_low_new_low(int n) const {
        TradeData* past = this->prev;
        for (int i = 0; i < n && (past != nullptr); i++, past = past->prev) {
            if (low > past->low)
                return false;
        }
        return true;
    }
    bool is_high_new_high_or_is_low_new_low_5days() const {
        return is_high_new_high_or_is_low_new_low_5days_;
    }
    bool is_high_new_high_5days() const {
        return is_high_new_high_5days_;
    }
    bool is_low_new_low_5days() const {
        return is_low_new_low_5days_;
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

struct SakataValuesPart1 {
    enum ss {
        SakataValuesPart1BitsN = 12
    };
    union {
        struct {
            // 3bit may cause padding by system
            // please put them padded like 2 2 4     4 2 2  no 424
            TradeData::SimpleBodyBodyPositionType body_body_position_type_today: 4;
            TradeData::CandleType candle_type_yesterday : 4;
            TradeData::CandleType candle_type_before_yesterday : 4;
        };
        uint32_t bits = 0;
    };
};
struct SakataValuesPart1Switch {
    union {
        struct {
            uint8_t body_body_position_type_today : 4;
            uint8_t candle_type_yesterday : 4;
            uint8_t candle_type_before_yesterday : 4;
        };
        uint32_t bits = 0;
    };
};







struct SakataValuesPart2 {
    enum SakataValuesPart2Enum {
        SakataValuesPart2BitsN = 10
    };
    union {
        struct {
            bool today_is_stick_up : 1;
            bool today_is_j_gt_k : 1;
            bool today_is_high_new_high_5days : 1;
            bool today_is_low_new_low_5days : 1;
            bool today_is_ma20_up : 1;
            bool today_is_ma60_up : 1;
            bool yesterday_is_high_new_high_5days : 1;
            bool yesterday_is_low_new_low_5days : 1;
            bool before_yesterday_is_high_new_high_5days : 1;
            bool before_yesterday_is_low_new_low_5days : 1;
        };
        uint32_t bits = 0;
    };
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
        std::vector<float> ma5_;
        //std::vector<float> ma10_;
        std::vector<float> ma20_;
        std::vector<float> ma60_;
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
        const std::vector<float>& ma5() const {
            return ma5_;
        }
        /*const std::vector<float>& ma10() const {
            return ma10_;
        }*/
        const std::vector<float>& ma20() const {
            return ma20_;
        }
        const std::vector<float>& ma60() const {
            return ma60_;
        }
        void InitKDJ(int n);
        void InitMA();
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
    std::vector<SakataValuesPart1> sakata_values_part1_;
    std::vector<SakataValuesPart2> sakata_values_part2_;
    std::vector<const TradeData*> trade_data_block_;
public: // property
    std::string industry_;
    const std::string& industry() const {
        return industry_;
    }
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
    const std::vector<SakataValuesPart1>& sakata_values_part1() const {
        return sakata_values_part1_;
    }
    const std::vector<SakataValuesPart2>& sakata_values_part2() const {
        return sakata_values_part2_;
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

        // update sakata values
        if (this->trade_data().size() < 10)
            return;

        sakata_values_part1_.clear();
        sakata_values_part1_.reserve(this->trade_data_.size());
        for (size_t i = 0; i < this->trade_data_.size(); i++) {
            SakataValuesPart1 sv;
            sakata_values_part1_.push_back(sv);
        }
        assert(sizeof(sakata_values_part1_[0].bits) == 4);
        sakata_values_part1_[0].bits = 0xffff; // this values is invalid
        sakata_values_part1_[1].bits = 0xffff;
        for (size_t i = 2; i < this->trade_data_.size(); i++) {
            sakata_values_part1_[i].body_body_position_type_today = this->trade_data()[i].body_body_position_type();
            sakata_values_part1_[i].candle_type_yesterday = this->trade_data()[i - 1].candle_type();
            sakata_values_part1_[i].candle_type_before_yesterday = this->trade_data()[i - 2].candle_type();
        }
        assert(sakata_values_part1_.size() >= 5);

        sakata_values_part2_.clear();
        sakata_values_part2_.reserve(this->trade_data_.size());
        for (size_t i = 0; i < this->trade_data_.size(); i++) {
            SakataValuesPart2 sv;
            sakata_values_part2_.push_back(sv);
        }
        assert(sizeof(sakata_values_part2_[0].bits) == 4);
        sakata_values_part2_[0].bits = 0xffff; // this values is invalid
        sakata_values_part2_[1].bits = 0xffff;
        for (size_t i = 2; i < this->trade_data_.size(); i++) {
            sakata_values_part2_[i].today_is_stick_up                       = this->trade_data()[i].is_stick_up();
            sakata_values_part2_[i].today_is_j_gt_k                         = this->indicators().kdj()[i].j > this->indicators().kdj()[i].k;
            sakata_values_part2_[i].today_is_high_new_high_5days            = this->trade_data()[i].is_high_new_high(5);
            sakata_values_part2_[i].today_is_low_new_low_5days              = this->trade_data()[i].is_low_new_low(5);
            sakata_values_part2_[i].today_is_ma20_up                        = this->indicators().ma20()[i] > this->indicators().ma20()[i - 1];
            sakata_values_part2_[i].today_is_ma60_up                        = this->indicators().ma60()[i] > this->indicators().ma60()[i - 1];
            sakata_values_part2_[i].yesterday_is_high_new_high_5days        = this->trade_data()[i - 1].is_high_new_high(5);
            sakata_values_part2_[i].yesterday_is_low_new_low_5days          = this->trade_data()[i - 1].is_low_new_low(5);
            sakata_values_part2_[i].before_yesterday_is_high_new_high_5days = this->trade_data()[i - 2].is_high_new_high(5);
            sakata_values_part2_[i].before_yesterday_is_low_new_low_5days   = this->trade_data()[i - 2].is_low_new_low(5);
        }
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
