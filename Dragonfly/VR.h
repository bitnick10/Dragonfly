#pragma once

#include "Stock.h"
#include "Indicator.h"

class VRCalculator {
public:
    double * av;
    double * bv;
    double * avs;
    double * bvs;

    const Stock& s;
    unsigned int n;

    VRCalculator(const Stock& s, unsigned int n);
    std::vector<float> Calculate();
    ~VRCalculator() {
        delete[] av;
        delete[] bv;
        delete[] avs;
        delete[] bvs;
    }
};