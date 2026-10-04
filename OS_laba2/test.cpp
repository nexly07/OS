#include "pch.h"
#include "test.h"

extern "C" __declspec(dllexport) void replaceMinMax(
    int* arr,
    unsigned int size,
    int minVal,
    int maxVal,
    double avg
) {
    int avgInt = static_cast<int>(avg);
    for (unsigned int i = 0; i < size; ++i) {
        if (arr[i] == minVal || arr[i] == maxVal) {
            arr[i] = avgInt;
        }
    }
}