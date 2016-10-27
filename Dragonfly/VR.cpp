#include "VR.h"

VRCalculator::VRCalculator(const Stock& s, unsigned int n): s(s), n(n) {
    av = new double[s.trade_data().size()];
    bv = new double[s.trade_data().size()];
    avs = new double[s.trade_data().size()];
    bvs = new double[s.trade_data().size()];
    memset(av, 0, sizeof(double)*s.trade_data().size());
    memset(bv, 0, sizeof(double)*s.trade_data().size());
    memset(avs, 0, sizeof(double)*s.trade_data().size());
    memset(bvs, 0, sizeof(double)*s.trade_data().size());
}

std::vector<float> VRCalculator::Calculate() {
    std::vector<float> vr;
    if (s.trade_data().size() == 0)
        return vr;
    vr.reserve(s.trade_data().size());

    for (size_t i = 1; i < s.trade_data().size(); i++) {
        if (s.trade_data()[i].percent_change() > 0) {
            av[i] = s.trade_data()[i].volume;
        } else {
            bv[i] = s.trade_data()[i].volume;
        }
    }
    indicator::SUM(avs, av, n, s.trade_data().size());
    indicator::SUM(bvs, bv, n, s.trade_data().size());
    vr.push_back(0.0);
    for (size_t i = 1; i < s.trade_data().size(); i++) {
        vr.push_back(avs[i] / bvs[i] * 100);
    }
    return std::move(vr);
}