#pragma once

#include <iostream>
#include <vector>

class Stock;

struct ARBR {
    enum ARType {
        Below70,
        Between70150,
        Above150,
        End
    };
    ARType ar_type;
    float ar = 0.0;
    float br = 0.0;
};

class ARBRCalculator {
public:
    const Stock& stock;
    int p;
    ARBRCalculator(const Stock& stock);
    std::vector<ARBR> GetResult();
    ~ARBRCalculator() {
        // DELETE_IF_NOT_NULLPTR(mids);
        //DELETE_IF_NOT_NULLPTR(sd);
    }
};