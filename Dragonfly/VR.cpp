#include "VR.h"
#include "Stock.h"
#include "Indicator.h"

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

std::vector<VR> VRCalculator::Calculate() {
    std::vector<VR> vrs;
    if (s.trade_data().size() == 0)
        return vrs;
    vrs.reserve(s.trade_data().size());

    for (size_t i = 1; i < s.trade_data().size(); i++) {
        if (s.trade_data()[i].percent_change() > 0) {
            av[i] = s.trade_data()[i].volume;
        } else {
            bv[i] = s.trade_data()[i].volume;
        }
    }
    indicator::SUM(avs, av, n, s.trade_data().size());
    indicator::SUM(bvs, bv, n, s.trade_data().size());
    VR onevr;
    onevr.vr = 0.0;
    vrs.push_back(onevr);
    for (size_t i = 1; i < s.trade_data().size(); i++) {
        VR vevalue;
        vevalue.vr = avs[i] / bvs[i] * 100;
        vrs.push_back(vevalue);
    }
    for (size_t i = 0; i < s.trade_data().size(); i++) {
        if (vrs[i].vr < 100) {
            vrs[i].vr_type = VR::VRType::Below100;
            continue;
        } else if (vrs[i].vr < 200) {
            vrs[i].vr_type = VR::VRType::Between100200;
            continue;
        } else {
            vrs[i].vr_type = VR::VRType::Above200;
            continue;
        }
    }
    return std::move(vrs);
}