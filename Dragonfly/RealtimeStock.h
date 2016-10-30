#pragma once

#include "Stock.h"

class RealtimeStock : public Stock {
public:
    year_month_day_hour_min_sec update_time;
    void Update() {
        trade_data_[trade_data().size() - 1].open = 1;
    }
};