#pragma once

extern "C" __declspec(dllexport) void replaceMinMax(
    int* arr,
    unsigned int size,
    int minVal,
    int maxVal,
    double avg
);