#pragma once

#include <iostream>
#include <vector>

class Stock;

#define DELETE_IF_NOT_NULLPTR(p)  if (p != nullptr) { delete[] p; p = nullptr; }

struct BOLL {
    float mid, upper, lower;
    float gap_percent() const {
        return (upper - mid) / mid;
    }
};

class BOLLCalculator {
public:
    // this can be easy to test and debug
    double* mids;
    double* sd;
    const Stock& s;
    int p;
    BOLLCalculator(const Stock& s, int n, int p);
    std::vector<BOLL> GetResult();
    ~BOLLCalculator() {
        DELETE_IF_NOT_NULLPTR(mids);
        DELETE_IF_NOT_NULLPTR(sd);
    }
};