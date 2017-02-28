#pragma once

#include <iostream>
#include <assert.h>

#define FMT_HEADER_ONLY
#include "fmt\format.h"

#include "DateTime.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN3(a,b,c) MIN(MIN(a,b),c)
#define MAX3(a,b,c) MAX(MAX(a,b),c)

#define TRADA_DATA_FLOAT float
struct Candlestick {
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

    enum class CandleType : uint8_t {
        FakeHollow,
        RealHollow,
        FakeFilled,
        RealFilled,
        End
    };

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

    Candlestick* prev;
    int i; // index self
    DateTime trade_time;
    TRADA_DATA_FLOAT open = 0, close = 0, high = 0, low = 0, volume = 0;
    //bool is_S_ST_StarST;
private:
    //bool is_stick_up_;
    //bool is_stick_filled_;
    //bool is_high_new_high_or_is_low_new_low_5days_;
    //bool is_high_new_high_5days_;
    //bool is_low_new_low_5days_;
    //TRADA_DATA_FLOAT body_up_line_;
    //TRADA_DATA_FLOAT body_bottom_line_;
    //TRADA_DATA_FLOAT upper_shadow_amplitude_;
    //TRADA_DATA_FLOAT lower_shadow_amplitude_;
    //TRADA_DATA_FLOAT amplitude_;
    //TRADA_DATA_FLOAT body_amplitude_;
    //SimpleCandleType simple_candle_type_;
    //CandleType candle_type_;
    //bool can_open_long_position_at_close_price_;
public: // property
    bool is_stick_up() const {
        return close > open;
    }
    bool is_stick_filled() const {
        return close < open;
    }
    bool is_open_jump() const {
        return abs((open - prev->close) / prev->close) > 1.0 / 100;
    }
    TRADA_DATA_FLOAT body_up_line() const {
        return MAX(open, close);
    }
    TRADA_DATA_FLOAT body_bottom_line() const {
        return MIN(open, close);
    }
    TRADA_DATA_FLOAT upper_shadow_amplitude() const {
        return (high - body_up_line()) / open;
    }
    TRADA_DATA_FLOAT lower_shadow_amplitude() const {
        return (MIN(open, close) - low) / open;
    }
    TRADA_DATA_FLOAT amplitude() const {
        return  abs(high - low) / open;
    }
    TRADA_DATA_FLOAT body_amplitude() const {
        return abs(open - close) / open;
    }
    TRADA_DATA_FLOAT body_amplitude_with_direction() const {
        return (close - open) / open;
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
            // if (is_stick_up()) {
            return lower_shadow_amplitude() > 2.0 / 100;
            //} else
            //     return upper_shadow_amplitude() > 2.0 / 100;
        } else {
            if (is_stick_up()) {
                return lower_shadow_amplitude() > body_amplitude();
            } else
                return upper_shadow_amplitude() > body_amplitude();
        }
    }
    bool has_long_head() const {
        if (body_amplitude() < 1.0 / 100) {
            // if (is_stick_up()) {
            return upper_shadow_amplitude() > 2.0 / 100;
            // } else
            //     return lower_shadow_amplitude() > 2.0 / 100;
        } else {
            if (is_stick_up()) {
                return upper_shadow_amplitude() > body_amplitude();
            } else
                return lower_shadow_amplitude() > body_amplitude();
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
    /* CandleType candle_type() const {
         return candle_type_;
     }*/
    // SimpleCandleType simple_candle_type() const {
    //    return simple_candle_type_;
    // }
    /* bool can_open_long_position_at_close_price() const {
         return can_open_long_position_at_close_price_;
     }*/
    double volume_ratio() const {
        return volume / prev->volume;
    }
public:
    std::string ToString() const {
        std::string ret = fmt::format("{0} {1:.2f} {2:.2f} {3:.2f} {4:.2f}", trade_time.ToString(), open, close, high, low);
        return ret;
    }
    //void Update() {
    //    is_stick_up_ = close > open;
    //    is_stick_filled_ = close < open;
    //    body_up_line_ = MAX(open, close);
    //    body_bottom_line_ = MIN(open, close);
    //    upper_shadow_amplitude_ = (high - body_up_line()) / open;
    //    lower_shadow_amplitude_ = (MIN(open, close) - low) / open;
    //    amplitude_ = abs(high - low) / open;
    //    body_amplitude_ = abs(open - close) / open;
    //    UpdateCandleType();
    //    //UpdateSimpleCandleType();
    //    UpdateCanOpenLongPositionAtClosePrice();
    //    is_high_new_high_or_is_low_new_low_5days_ = is_high_new_high(5) || is_low_new_low(5);
    //    is_high_new_high_5days_ = is_high_new_high(5);
    //    is_low_new_low_5days_ = is_low_new_low(5);
    //}
private:
    SimpleBodyPositionType GetSimpleBodyPositionType(const Candlestick& d, TRADA_DATA_FLOAT price) const {
        if (price - d.body_up_line() > 0.0001)
            return SimpleBodyPositionType::Above;
        else if (price - d.body_bottom_line() < -0.0001) {
            return SimpleBodyPositionType::Below;
        } else {
            return SimpleBodyPositionType::In;
        }
    }
    /* void UpdateCandleType() {
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
     }*/
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
    /*void UpdateCanOpenLongPositionAtClosePrice() {
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
    }*/
public:
    Candlestick() {
    }
    bool is_close_new_high(int n) const {
        Candlestick* past = this->prev;
        for (int i = 0; i < n && (past != nullptr); i++, past = past->prev) {
            if (close < past->high)
                return false;
        }
        return true;
    }
    bool is_high_new_high(int n) const {
        Candlestick* past = this->prev;
        for (int i = 0; i < n && (past != nullptr); i++, past = past->prev) {
            if (high < past->high)
                return false;
        }
        return true;
    }
    bool is_low_new_low(int n) const {
        Candlestick* past = this->prev;
        for (int i = 0; i < n && (past != nullptr); i++, past = past->prev) {
            if (low > past->low)
                return false;
        }
        return true;
    }
    /* bool is_high_new_high_or_is_low_new_low_5days() const {
         return is_high_new_high_or_is_low_new_low_5days_;
     }
     bool is_high_new_high_5days() const {
         return is_high_new_high_5days_;
     }
     bool is_low_new_low_5days() const {
         return is_low_new_low_5days_;
     }*/
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
    //void set_begin_time(year_month_day_hour_min_sec time) {
    //    this->begin_time = time;
    //}
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
        const Candlestick* day_before = this;
        for (int i = 0; i < n; i++) {
            day_before = day_before->prev;
        }
        return (close - day_before->close) > 0;
    }
    bool is_price_down(int n) const {
        // 0 today
        // 1 yestday
        const Candlestick* day_before = this;
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

inline Candlestick Combine(const Candlestick& stick1, const Candlestick& stick2) {
    Candlestick stick;
    stick.trade_time = stick2.trade_time;
    stick.open = stick1.open;
    stick.close = stick2.close;
    stick.high = MAX(stick1.high, stick2.high);
    stick.low = MIN(stick1.low, stick2.low);
    stick.volume = stick1.volume + stick2.volume;
    return stick;
}