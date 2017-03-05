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

class POI {
public:
    const Chart* chart = nullptr;
    int n;
    enum class Tag {
        NewHigh, NewLow, None
    };
    std::vector<Tag> tags;
    std::vector<float> pos;
    POI(const Chart* pchart, int n);
    int NearestTag(int data_i) const {
        for (int i = data_i; i >= 0; i--) {
            if (tags[i] != Tag::None)
                return i;
        }
        return -1;
    }
    int NearestTag(int data_i, Tag t) const {
        for (int i = data_i; i >= 0; i--) {
            if (tags[i] == t)
                return i;
        }
        return -1;
    }
    Tag OppsiteTag(Tag t) const {
        if (t == Tag::NewHigh)
            return Tag::NewLow;
        if (t == Tag::NewLow)
            return Tag::NewHigh;
        else
            return Tag::None;
    }
    float GetPos(int data_i);
};