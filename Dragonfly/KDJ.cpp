#pragma once

#include "KDJ.h"

#include "Indicator.h"
#include "Stock.h"

//bool KDJ::IsGoldenCross(const year_month_day_hour_min_sec& time) {
//    int i = s->GetIndex(time);
//    if (i == -1) {
//        return false;
//    }
//    kdj last = kdjs[i];
//    kdj previous = kdjs[i - 1];
//    if (last.j > previous.j
//            && last.j >= last.k
//            && last.j >= last.d
//            && previous.k > previous.j
//            && previous.d > previous.j) {
//        return true;
//    }
//    return false;
//}
//bool KDJ::WillGoldenCross(const year_month_day_hour_min_sec& time) {
//    int i = s->GetIndex(time);
//    if (i == -1) {
//        return false;
//    }
//    kdj today = kdjs[i];
//    kdj previous = kdjs[i - 1];
//    auto tomorrowJ = today.j + today.j - previous.j;
//    if (tomorrowJ > today.j
//            && tomorrowJ >= today.k
//            && tomorrowJ >= today.d
//            && today.k > today.j
//            && today.d > today.j) {
//        return true;
//    }
//    return false;
//}
//KDJ::~KDJ() {
//    if (kdjs != nullptr) {
//        delete[] kdjs;
//        kdjs = nullptr;
//    }
//}
