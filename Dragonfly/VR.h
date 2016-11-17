#pragma once

#include <iostream>
#include <vector>

class Stock;

struct VR {
    enum VRType {
        Below100,
        Between100200,
        Above200,
        End
    };
    VRType vr_type;
    float vr;
};
class VRCalculator {
public:
    double * av;
    double * bv;
    double * avs;
    double * bvs;

    const Stock& s;
    unsigned int n;

    VRCalculator(const Stock& s, unsigned int n);
    std::vector<VR> Calculate();
    ~VRCalculator() {
        delete[] av;
        delete[] bv;
        delete[] avs;
        delete[] bvs;
    }
};