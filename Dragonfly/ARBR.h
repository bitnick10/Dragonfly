#pragma once

#include <iostream>
#include <vector>

class Stock;

struct ARBR {
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