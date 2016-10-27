#pragma once

#include <vector>

#pragma warning(disable: 4244) // warning C4244: conversion from 'double' to 'float', possible loss of data

class KDJ {
public:
    float k, d, j;
    KDJ(double k, double d, double j) {
        this->k = k;
        this->d = d;
        this->j = j;
    }
};