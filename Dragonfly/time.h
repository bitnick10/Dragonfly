#pragma once

#include <ctime>
#include <iostream>
#include <assert.h>

#pragma warning(disable: 4996) // Error	C4996	'sprintf': This function or variable may be unsafe.Consider using sprintf_s instead.To disable deprecation, use _CRT_SECURE_NO_WARNINGS.See online help for details.

struct year_month_day_hour_min_sec {
private:
    std::time_t epoch_;
public:
    std::time_t epoch() const {
        return epoch_;
    }
    void set_epoch(const std::time_t& epoch) {
        this->epoch_ = epoch;
    }
    year_month_day_hour_min_sec() {
        epoch_ = time(nullptr); // time now
    }
    // days since Sunday - [0, 6]
    int weekday() const {
        tm* lt = std::localtime(&epoch_);
        return lt->tm_wday;
    }
    year_month_day_hour_min_sec(uint16_t year, uint8_t month, uint8_t day, uint8_t hour = 0, uint8_t minute = 0, uint8_t second = 0) {
        tm time_in = { second, minute, hour, day, month - 1, year - 1900 };
        this->epoch_ = std::mktime(&time_in);
    }
    void AddDay(int n) {
        epoch_ += n * 24 * 60 * 60;
    }
    void AddHour(int n) {
        epoch_ += n * 60 * 60;
    }
    void AddMinutes(int n) {
        epoch_ += n * 60;
    }
    std::time_t ToStdTime() {
        return epoch_;
    }
    std::string ToString(const char* format) {
        auto stdtime = ToStdTime();
        char buffer[80];
        tm* tm_info;
        tm_info = std::localtime(&stdtime);

        strftime(buffer, 80, format, tm_info);
        return std::string(buffer);
    }
    std::string ToString() const {
        char buffer[80];
        tm* tm_info;
        tm_info = std::localtime(&epoch_);

        strftime(buffer, 80, "%F %T", tm_info);
        return std::string(buffer);
    }
    std::string ToYYYYMMDDString() const {
        char buffer[80];
        tm* tm_info;
        tm_info = std::localtime(&epoch_);

        strftime(buffer, 80, "%Y%m%d", tm_info);
        return std::string(buffer);
    }
    year_month_day_hour_min_sec& operator =(const year_month_day_hour_min_sec& rhs) {
        this->epoch_ = rhs.epoch_;
        return *this;
    }

    inline bool operator==(const year_month_day_hour_min_sec& rhs) const {
        return this->epoch_ == rhs.epoch_;
    }
    inline bool operator!=(const year_month_day_hour_min_sec& rhs) const {
        return !this->operator==(rhs);
    }
    inline bool operator <(const year_month_day_hour_min_sec& rhs) const {
        return epoch_ < rhs.epoch_;
    }
    friend std::ostream& operator<<(std::ostream& out, const year_month_day_hour_min_sec& rhs) {
        char buffer[80];
        tm* tm_info;
        tm_info = std::localtime(&rhs.epoch_);

        strftime(buffer, 80, "%F %T", tm_info);
        out << buffer;
        return out;
    }
};

struct eight_digit_time {
private:
    int value_;
public:
    int value() const {
        return value_;
    }
    eight_digit_time(int value) {
        assert(19000101 < value && value < 99991231);
        this->value_ = value;
    }
    // add day
    eight_digit_time operator+ (int n) {
        year_month_day_hour_min_sec ymd = *this;
        ymd.AddDay(n);
        std::time_t tm = ymd.epoch();
        struct tm *aTime = localtime(&tm);

        int day = aTime->tm_mday;
        int month = aTime->tm_mon + 1; // Month is 0 - 11, add 1 to get a jan-dec 1-12 concept
        int year = aTime->tm_year + 1900; // Year is # years since 1900
        eight_digit_time ret = year * 10000 + month * 100 + day;
        return ret;
    }
    eight_digit_time operator- (int n) {
        return operator+(-n);
    }
    operator year_month_day_hour_min_sec() {
        std::tm time_in = { 0, 0, 0, // second, minute, hour
                            value_ % 100, value_ / 100 % 100 - 1, value_ / 10000 - 1900
                          }; // 1-based day, 0-based month, year since 1900
        std::time_t temp = std::mktime(&time_in);
        year_month_day_hour_min_sec ret;
        ret.set_epoch(temp);
        return ret;
    }
    friend std::ostream& operator<<(std::ostream& out, const eight_digit_time& rhs) {
        out << rhs.value();
        return out;
    }
};
