#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

#include "Market.h"
#include "KDJ.h"
#include "Indicator.h"
#define FMT_HEADER_ONLY
#include "fmt/format.h"

#pragma warning(disable: 4521; disable: 4522)

struct Wedge {
public:
    union {
        struct {
            char ar_sign; // -1 means down, 0 means equal, 1 means up
            char br_sign; // -1 means down, 0 means equal, 1 means up
            char kdj_sign; // -1 means down, 0 means equal, 1 means up
            char asi_sign;
            char vr_sign;
            char ma_order_n[6];
        };
        unsigned __int32 signs[3];
    };
    float kdj_j_delta_abs;
    std::tuple<float, float> kdj_j_delta_abs_interval;
    // not cool no use now
    //int dmi_d1_sign = 0;
    //int dmi_d2_sign = 0;
    //int dmi_adx_sign = 0;
    //int dmi_adxr_sign = 0;
    struct MA {
        int n;
        double value;
    };
    std::string id() const {
        std::string ret;
        ret = std::to_string(ar_sign) + std::to_string(br_sign) + std::to_string(kdj_sign) + std::to_string(asi_sign) + std::to_string(vr_sign);
        ret += std::to_string(ma_order_n[0]) + std::to_string(ma_order_n[1]) + std::to_string(ma_order_n[2]) + std::to_string(ma_order_n[3]) + std::to_string(ma_order_n[4]) + std::to_string(ma_order_n[5]);
        ret += std::to_string((int)(kdj_j_delta_abs * 100));
        return ret;
    }
    Wedge(const Wedge& other) = delete;
    Wedge(const Wedge&& other) {
        this->signs[0] = other.signs[0];
        this->signs[1] = other.signs[1];
        this->signs[2] = other.signs[2];
        this->kdj_j_delta_abs = other.kdj_j_delta_abs;
        this->kdj_j_delta_abs_interval = other.kdj_j_delta_abs_interval;
    }
    Wedge() {};
    Wedge(const TradeData& d, const Stock& s) {
        assert(sizeof(bool) == 1);
        this->signs[0] = 0;
        this->signs[1] = 0;
        this->signs[2] = 0;

        std::vector<MA> ma_order;
        MA ma1, ma5, ma10, ma20, ma30, ma60;// , ma120, ma240;
        ma1.n = 1;
        ma1.value = d.close;
        ma5.n = 5;
        ma5.value = indicator::MA(d, 5);
        ma10.n = 10;
        ma10.value = indicator::MA(d, 10);
        ma20.n = 20;
        ma20.value = indicator::MA(d, 20);
        ma30.n = 30;
        ma30.value = indicator::MA(d, 30);
        ma60.n = 60;
        ma60.value = indicator::MA(d, 60);
        /* ma120.n = 120;
         ma120.value = indicator::MA(d, 120);
         ma240.n = 240;
         ma240.value = indicator::MA(d, 240);*/
        ma_order.push_back(ma1);
        ma_order.push_back(ma5);
        ma_order.push_back(ma10);
        ma_order.push_back(ma20);
        ma_order.push_back(ma30);
        ma_order.push_back(ma60);
        /* ma_order.push_back(ma120);
         ma_order.push_back(ma240);*/
        assert(ma_order.size() == 6);
        std::sort(ma_order.begin(), ma_order.end(),
        [](const MA & a, const MA & b) -> bool {
            return a.value < b.value;
        });
        for (size_t i = 0; i < ma_order.size(); i++) {
            ma_order_n[i] = ma_order[i].n;
        }
        int i = s.GetIndex(d.begin_time);
        ar_sign = GetSign(s.indicators().arbr()[i].ar, s.indicators().arbr()[i - 1].ar);
        br_sign = GetSign(s.indicators().arbr()[i].br, s.indicators().arbr()[i - 1].br);
        kdj_sign = GetSign(s.indicators().kdj()[i].j, s.indicators().kdj()[i - 1].j);
        asi_sign = GetSign(s.indicators().asi()[i], s.indicators().asi()[i - 1]);
        kdj_j_delta_abs = abs(s.indicators().kdj()[i].j - s.indicators().kdj()[i - 1].j);
        kdj_j_delta_abs_interval = std::make_tuple(float(kdj_j_delta_abs * 0.382), float(kdj_j_delta_abs * 2.618));

        // VR Sign
        if (s.indicators().vr()[i] > 350) {
            this->vr_sign = 3;
        } else if (s.indicators().vr()[i] > 200) {
            this->vr_sign = 2;
        } else if (s.indicators().vr()[i] > 100) {
            this->vr_sign = 1;
        } else {
            this->vr_sign = 0;
        }
    }
    int GetSign(double a, double b) {
        if (AreSame(a, b))
            return 0;
        else {
            if (a > b)
                return 1;
            else
                return -1;
        }
    }
    bool AreSame(double a, double b) {
        return abs(a - b) < 0.001;
    }
    bool operator==(const Wedge & rhs) const {
        if (this->signs[0] == rhs.signs[0] && this->signs[1] == rhs.signs[1] && this->signs[2] == rhs.signs[2] && this->IsKDJJFit(rhs)) {
            return true;
        } else {
            return false;
        }
    }
    bool IsKDJJFit(const Wedge & rhs) const {
        //The RSV can output power of the amplitude of the body
        // and the amplitude of the body fits is if the small one 1,the biger one is 1 to 1.618
        // 1.618*1.618=2.618
        // 1/2.618=0.382
        if (rhs.kdj_j_delta_abs < std::get<0>(kdj_j_delta_abs_interval) || rhs.kdj_j_delta_abs > std::get<1>(kdj_j_delta_abs_interval)) {
            return false;
        } else {
            return true;
        }
        /* double big_one = MAX(abs(this->kdj_j_delta_abs), abs(rhs.kdj_j_delta_abs));
         double small_one = MIN(abs(this->kdj_j_delta_abs), abs(rhs.kdj_j_delta_abs));
         if (big_one > small_one * 2.618) {
        	 return false;
         } else {
        	 return true;
         }*/
    }
};
struct Tenon {
    enum EightLocation : byte {
        LowerThanLow,
        InLowerShadow,
        EqualBodyBottomLine,
        InUpHalfBody,
        InDownHalfBody,
        EqualBodyUpLine,
        InHighShadow,
        HigherThanHigh
    };
    //enum SixLocation : byte {
    //    LowerThanLow,
    //    InLowerShadow,
    //    InUpHalfBody,
    //    InDownHalfBody,
    //    InHighShadow,
    //    HigherThanHigh
    //};
public:
    int GetSign(double a, double b) {
        if (AreSame(a, b))
            return 0;
        else {
            if (a > b)
                return 1;
            else
                return -1;
        }
    }
    EightLocation GetLocation(const TradeData& d, double price) {
        double low = d.low;
        double body_bottom_line = d.open;
        double body_middle = (d.open + d.close) / 2;
        double body_up_line = d.close;
        double high = d.high;
        if (d.percent_change_today() < 0) {
            body_bottom_line = d.close;
            body_up_line = d.open;
        }
        if (price <= body_middle) {
            if (price < low)
                return EightLocation::LowerThanLow;
            else if (price < body_bottom_line)
                return EightLocation::InLowerShadow;
            else if (AreSame(price, body_bottom_line))
                return EightLocation::EqualBodyBottomLine;
            else
                return EightLocation::InDownHalfBody;
        } else {
            if (price > high)
                return EightLocation::HigherThanHigh;
            else if (price > body_up_line)
                return EightLocation::InHighShadow;
            else if (AreSame(price, body_up_line))
                return EightLocation::EqualBodyUpLine;
            else
                return EightLocation::InUpHalfBody;
        }
    }
    EightLocation GetSixLocation(const TradeData& d, double price) {
        double low = d.low;
        double body_bottom_line = d.open;
        double body_middle = (d.open + d.close) / 2;
        double body_up_line = d.close;
        double high = d.high;
        if (d.percent_change_today() < 0) {
            body_bottom_line = d.close;
            body_up_line = d.open;
        }
        if (price <= body_middle) {
            if (price < low)
                return EightLocation::LowerThanLow;
            else if (price < body_bottom_line)
                return EightLocation::InLowerShadow;
            else
                return EightLocation::InDownHalfBody;
        } else {
            if (price > high)
                return EightLocation::HigherThanHigh;
            else if (price > body_up_line)
                return EightLocation::InHighShadow;
            else
                return EightLocation::InUpHalfBody;
        }
    }
    int GetVolumeIntervalIndex(const TradeData& d) {
        double volume_scale = d.volume / d.prev->volume;
        double interval[] = { 0.2, 0.33, 0.5, 0.66, 0.8, 0.93, 1.07, 1.25, 1.5, 2, 3, 5 };
        int i = 0;
        for (; i < sizeof(interval) / sizeof(*interval); i++) {
            if (volume_scale < interval[i]) {
                return i;
            }
        }
        return i;
    }
    bool AreSame(double a, double b) {
        return abs(a - b) < 0.001;
    }

};
struct AmplitudeTenon {
    AmplitudeTenon() {}
    AmplitudeTenon(const TradeData& d) {
        this->body_amplitude = d.body_amplitude();
        this->upper_shadow_amplitude = d.upper_shadow_amplitude();
        this->lower_shadow_amplitude = d.lower_shadow_amplitude();
        this->body_amplitude_fit_range = GetAmplitudeFitRange(body_amplitude, 0.5 / 100, 1.618 / 100, 0.618, 1.618);
        this->upper_shadow_amplitude_fit_range = GetAmplitudeFitRange(upper_shadow_amplitude, 0.7 / 100, 1.618 / 100, 0.618, 1.618);
        this->lower_shadow_amplitude_fit_range = GetAmplitudeFitRange(lower_shadow_amplitude, 0.7 / 100, 1.618 / 100, 0.618, 1.618);
    }
    float body_amplitude;
    float upper_shadow_amplitude;
    float lower_shadow_amplitude;
    std::pair<float, float> body_amplitude_fit_range;
    std::pair<float, float> upper_shadow_amplitude_fit_range;
    std::pair<float, float> lower_shadow_amplitude_fit_range;
    std::string id() const {
        std::string ret;
        ret = std::to_string((int)(this->body_amplitude * 1000)) + std::to_string((int)(this->upper_shadow_amplitude * 1000)) + std::to_string((int)(this->lower_shadow_amplitude * 1000));
        return ret;
    }
    bool IsFit(const AmplitudeTenon& other) const {
        if (IsAmplitudeNotInFitRange(other.body_amplitude, this->body_amplitude_fit_range)) {
            return false;
        }
        if (IsAmplitudeNotInFitRange(other.upper_shadow_amplitude, this->upper_shadow_amplitude_fit_range)) {
            return false;
        }
        if (IsAmplitudeNotInFitRange(other.lower_shadow_amplitude, this->lower_shadow_amplitude_fit_range)) {
            return false;
        }
        /* if (!IsAmplitudeFit(this->body_amplitude, other.body_amplitude, 0.5 / 100, 0.618, 1.618)) {
             return false;
         }
         if (!IsAmplitudeFit(this->upper_shadow_amplitude, other.upper_shadow_amplitude, 0.7 / 100, 0.618, 1.618)) {
             return false;
         }
         if (!IsAmplitudeFit(this->lower_shadow_amplitude, other.lower_shadow_amplitude, 0.7 / 100, 0.618, 1.618)) {
             return false;
         }*/
        return true;
    }
private:
    /* inline bool IsAmplitudeFit(double amp1, double amp2, double delta, double lower_scale, double upper_scale) const {
         assert(lower_scale <= 1);
         assert(upper_scale >= 1);
         if (abs(amp1 - amp2) < delta)
             return true;
         if (amp1 > (amp2 * upper_scale) || amp1 < (amp2 * lower_scale)) {
             return false;
         } else {
             return true;
         }
     }*/
    inline std::pair<float, float> GetAmplitudeFitRange(float amplitude, double base_delta, double max_scale, double lower_scale, double upper_scale) const {
        assert(lower_scale <= 1);
        assert(upper_scale >= 1);
        float l1 = amplitude - base_delta;
        float l2 = amplitude * lower_scale;
        //float l3;// = amplitude - max_scale;
        if (amplitude - l2 > max_scale) {
            l2 = amplitude - max_scale;
        }
        float u1 = amplitude + base_delta;
        float u2 = amplitude * upper_scale;
        if (u2 - amplitude > max_scale) {
            u2 = amplitude + max_scale;
        }
        std::pair<float, float> ret;
        if (l1 < l2) {
            ret.first = l1;
        } else {
            ret.first = l2;
        }
        if (u1 > u2) {
            ret.second = u1;
        } else {
            ret.second = u2;
        }
        return ret;
    }
    inline bool IsAmplitudeInFitRange(float amplitude, const std::pair<float, float>& range) const {
        if (range.first < amplitude && amplitude < range.second) {
            return true;
        } else {
            return false;
        }
    }
    inline bool IsAmplitudeNotInFitRange(float amplitude, const std::pair<float, float>& range) const {
        if (amplitude < range.first || amplitude > range.second) {
            return true;
        } else {
            return false;
        }
    }
};
struct TenonAVP : public Tenon {
    union {
        struct {
            byte open_type;
            byte close_type;
            bool is_high_new_high; // this must, if high is new high the meaning is different
            bool is_low_new_low;
            bool is_stick_up;
            bool is_price_up;
        };
        __int32 signs[2];
    };
    AmplitudeTenon amplitude_tenon;
    int volume_interval_i;
    float volume_scale;
    std::string id() const {
        std::string ret;
        ret = std::to_string(open_type) + std::to_string(close_type) + std::to_string(is_high_new_high) + std::to_string(is_low_new_low);
        ret += std::to_string(is_stick_up) + std::to_string(is_price_up);
        ret += amplitude_tenon.id();
        ret += std::to_string(volume_interval_i) + std::to_string((int)(volume_scale * 10000));
        return ret;
    }

    TenonAVP() {}
    TenonAVP(TenonAVP& other) = delete;
    TenonAVP(const TenonAVP& other) = delete;
    TenonAVP& operator=(TenonAVP &) = delete;
    TenonAVP& operator=(const TenonAVP &) = delete;

    TenonAVP(TenonAVP&& other) {
        this->signs[0] = other.signs[0];
        this->signs[1] = other.signs[1];
        this->volume_interval_i = other.volume_interval_i;
        this->amplitude_tenon = other.amplitude_tenon;
        this->volume_scale = other.volume_scale;
    }

    ~TenonAVP() {}
    TenonAVP(const TradeData& d): amplitude_tenon(d) {
        assert(sizeof(bool) == 1);
        assert(sizeof(char) == 1);

        this->signs[0] = 0;
        this->signs[1] = 0;

        double body_bottom_line = d.open;
        double body_up_line = d.close;
        if (d.percent_change_today() < 0) {
            body_bottom_line = d.close;
            body_up_line = d.open;
        }

        if (AreSame(d.volume, 0.0) || AreSame(d.prev->volume, 0.0)) {
            volume_scale = -1;
        } else {
            volume_scale = d.volume / d.prev->volume;
        }
        volume_interval_i = GetVolumeIntervalIndex(d);
        //has_upper_shadow = d.has_upper_shadow();
        // has_lower_shadow = d.has_lower_shadow();
        is_stick_up = d.is_stick_up();
        is_price_up = d.is_price_up();

        open_type = GetLocation(*d.prev, d.open);
        close_type = GetLocation(*d.prev, d.close);
        is_high_new_high = d.high > d.prev->high ? true : false;
        is_low_new_low = d.low < d.prev->low ? true : false;

        /* if (abs((d.open - d.prev->close) / d.prev->close) < 0.1 / 100) {
             if (d.prev->is_stick_up()) {
                 open_type = Tenon::EightLocation::EqualBodyUpLine;
             } else {
                 open_type = Tenon::EightLocation::EqualBodyBottomLine;
             }
         }*/
    }
    bool IsFit(const TenonAVP& rhs) const {
        if (this->signs[0] == rhs.signs[0] && this->signs[1] == rhs.signs[1]) {
            if (!IsVolumeFit(rhs))
                return false;
            if (!amplitude_tenon.IsFit(rhs.amplitude_tenon))
                return false;
            return true;
        } else {
            return false;
        }
    }
    bool IsVolumeFit(const TenonAVP& rhs) const {
        if (abs(this->volume_scale - rhs.volume_scale) < 0.07 || this->volume_interval_i == rhs.volume_interval_i)
            return true;
        else
            return false;
    }
};
struct TenonAV : public Tenon {
    bool is_stick_up;

    AmplitudeTenon amplitude_tenon;

    float volume_scale;
    int volume_interval_i;
    // bool is_price_up;
    // find that the is_price_up killed a lot of good stock,don't know why
    // Think that the battle is in the candlestick, volume links the stick not the yestoday price
    // only the open price has relationship between yestoday's close price,then with the open price,a new battle begin
    // open price may affect by the big market or some random things witch has no relationship with this stock
    // if the market is now well ,more than 80% stock open price low,but if the stock want up today,it up today,lower open price should exclude.
    // but if longs think that the stock is good,the price will up today

    TenonAV() {}
    TenonAV(TenonAV& other) = delete;
    TenonAV(const TenonAV& other) = delete;
    TenonAV& operator=(TenonAV &) = delete;
    TenonAV& operator=(const TenonAV &) = delete;

    TenonAV(const TradeData& d) : amplitude_tenon(d) {
        //assert(d.prev != nullptr);
        this->is_stick_up = d.is_stick_up();

        if (AreSame(d.volume, 0.0) || AreSame(d.prev->volume, 0.0)) {
            volume_scale = -1;
        } else {
            volume_scale = d.volume / d.prev->volume;
        }
        volume_interval_i = GetVolumeIntervalIndex(d);
    }
    TenonAV(TenonAV&& other) {
        this->is_stick_up = other.is_stick_up;

        this->amplitude_tenon = other.amplitude_tenon;

        this->volume_scale = other.volume_scale;
        this->volume_interval_i = other.volume_interval_i;
    }
    std::string id() const {
        std::string ret;
        ret += std::to_string(is_stick_up);
        ret += amplitude_tenon.id();
        ret += std::to_string(volume_interval_i) + std::to_string((int)(volume_scale * 10000));
        return ret;
    }
    bool IsFit(const TenonAV& rhs) const {
        if (this->is_stick_up == rhs.is_stick_up) {
            if (!IsVolumeFit(rhs))
                return false;
            if (!amplitude_tenon.IsFit(rhs.amplitude_tenon))
                return false;
            return true;
        } else {
            return false;
        }
    }
    bool IsVolumeFit(const TenonAV& rhs) const {
        if (abs(this->volume_scale - rhs.volume_scale) < 0.07 || this->volume_interval_i == rhs.volume_interval_i)
            return true;
        else
            return false;
    }
};

struct StockAndIndex {
    StockAndIndex(const Stock* s, int i) {
        this->stock = s;
        this->i = i;
    }
    const Stock* stock = nullptr;
    int i = -1;
};
class IMTI {
public:
    IMTI() {}
    virtual std::vector<StockAndIndex> FindFit(const Market& m) const = 0;
};
class MTI3: public IMTI {
public:
    Wedge w;
    TenonAVP tenon1;
    TenonAVP tenon2;
    TenonAV tenon3;
public:
    MTI3() {}
    MTI3(MTI3& other) = delete;
    MTI3(const MTI3& other) = delete;
    MTI3& operator=(MTI3 &) = delete;
    MTI3& operator=(const MTI3 &) = delete;
    MTI3(MTI3&& other) : w(std::move(other.w)), tenon1(std::move(other.tenon1)), tenon2(std::move(other.tenon2)), tenon3(std::move(other.tenon3)) {
    }
    MTI3(const TradeData& d, const Stock& s) : w(d, s), tenon1(d), tenon2(*d.prev), tenon3(*(d.prev->prev)) {
        assert(d.prev != nullptr);
        assert(d.prev->prev != nullptr);
        assert(d.prev->prev->prev != nullptr);
        assert(s.trade_data().size() >= 10);
        //std::cout << s.id() << "\n";
    }
    std::string id() const {
        return tenon1.id() + tenon2.id() + tenon3.id() + w.id();
    }
    bool IsFit(const MTI3& other) const {
        if (w == other.w && tenon1.IsFit(other.tenon1) && tenon2.IsFit(other.tenon2) && tenon3.IsFit(other.tenon3))
            return true;
        else
            return false;
    }
    // you should remove same stock with index after call this function except i = last index
    virtual std::vector<StockAndIndex> FindFit(const Market& m) const {
        std::vector<StockAndIndex> ret;
        for (auto& s : m.stocks) {
            if (s->trade_data().size() > 10) {
                for (size_t i = 5; i < s->trade_data().size() - 2; ++i) {
                    const MTI3& other = s->indicators().mti3()[i];
                    if (this->IsFit(other)) {
                        //std::cout << "find " << s.id() << " at " << i << "\n";

                        ret.push_back(StockAndIndex(s.get(), i));
                    }
                }
            }
        }
        return std::move(ret);
    }
};
class MTI2 : public IMTI {
public:
    Wedge w;
    TenonAVP tenon1;
    TenonAV tenon2;
public:
    MTI2() {}
    MTI2(MTI2& other) = delete;
    MTI2(const MTI2& other) = delete;
    MTI2& operator=(MTI2 &) = delete;
    MTI2& operator=(const MTI2 &) = delete;
    MTI2(MTI2&& other) : w(std::move(other.w)), tenon1(std::move(other.tenon1)), tenon2(std::move(other.tenon2)) {
    }
    MTI2(const TradeData& d, const Stock& s) : w(d, s), tenon1(d), tenon2(*d.prev) {
        assert(d.prev != nullptr);
        assert(d.prev->prev != nullptr);
        assert(d.prev->prev->prev != nullptr);
        assert(s.trade_data().size() >= 10);
        //std::cout << s.id() << "\n";
    }
    std::string id() const {
        return tenon1.id() + tenon2.id() + w.id();
    }
    bool IsFit(const MTI2& other) const {
        if (w == other.w && tenon1.IsFit(other.tenon1) && tenon2.IsFit(other.tenon2))
            return true;
        else
            return false;
    }
    virtual std::vector<StockAndIndex> FindFit(const Market& m) const {
        std::vector<StockAndIndex> ret;
        for (auto& s : m.stocks) {
            if (s->trade_data().size() > 10) {
                for (size_t i = 5; i < s->trade_data().size() - 2; ++i) {
                    const MTI2& other = s->indicators().mti2()[i];
                    if (this->IsFit(other)) {
                        //std::cout << "find " << s.id() << " at " << i << "\n";
                        ret.push_back(StockAndIndex(s.get(), i));
                    }
                }
            }
        }
        return std::move(ret);
    }
};
// Mortise and Tenon Indicator
