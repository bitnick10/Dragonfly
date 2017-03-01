#pragma once

#include <iostream>
#include <vector>
#include <cassert>

#define DELETE_IF_NOT_NULLPTR(p)  if (p != nullptr) { delete[] p; p = nullptr; }

class Chart;

struct MACD {
    float macd, diff, dea;
};

class MACDCalculator {
public:
    static std::vector<MACD> GetResult(const Chart& chart, int short_n, int long_n, int dea_n);
};

class KDJ {
public:
    const Chart* chart = nullptr; // father
    int n = 0, sn1 = 0, sn2 = 0;
    std::vector<float> k;
    std::vector<float> d;
    //std::vector<float> j;
    int id() const {
        assert(n < 1000);
        assert(sn1 < 1000);
        assert(sn2 < 1000);
        return n * 1000 * 1000 + sn1 * 1000 + sn2;
    }
    //KDJ(const KDJ&) = delete;
    KDJ(const Chart* chart, int n, int sn1, int sn2, bool is_allocate_memory = true, bool is_init_data = true);
    void Init();
    //void Init(const Chart& chart, int n, int sn1, int sn2);
};

class BOLL {
public:
    const Chart* chart = nullptr;
    std::vector<float> mid;
    std::vector<float> upper;
    std::vector<float> lower;
    int n = 0, p = 0;
    BOLL(const Chart* pchart, int n, int p);
};

//class BOLLCalculator {
//public:
//	// this can be easy to test and debug
//	double* mids;
//	double* sd;
//	const Stock& s;
//	int p;
//	BOLLCalculator(const Stock& s, int n, int p);
//	std::vector<BOLL> GetResult();
//	~BOLLCalculator() {
//		DELETE_IF_NOT_NULLPTR(mids);
//		DELETE_IF_NOT_NULLPTR(sd);
//	}
//};