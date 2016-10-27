#pragma once

#include <vector>

#include "Indicator.h"
#include "BOLL.h"

class Stock;

//class ASI {
//public:
//    Stock* s;
//    double* asis;
//    ASI(Stock* s);
//    bool IsLeadingBreakthrough(eight_digit_time date, int nDays);
//    ~ASI();
//};

// Accumulative Swing Index
// http://baike.baidu.com/view/349569.htm
// http://fxcodebase.com/wiki/index.php/Accumulation_Swing_Index_(ASI)
class ASICalculator {
    float *si;
    float *asi;
    const Stock& s;
    int n;
public:
    ASICalculator(const Stock& s, int n);
    std::vector<float> GetResult();
    ~ASICalculator() {
        DELETE_IF_NOT_NULLPTR(si);
        DELETE_IF_NOT_NULLPTR(asi);
    }
};