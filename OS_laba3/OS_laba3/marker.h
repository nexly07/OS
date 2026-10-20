#pragma once

#define NOMINMAX
#include <windows.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstdint>

using namespace std;

struct MarkerState {
    int* arr = nullptr;
    int arraySize = 0;
    int threadCount = 0;
    bool silent = false;
    CRITICAL_SECTION cs;
    HANDLE hStartEvent = NULL;
    vector<HANDLE> hThreads;
    vector<HANDLE> hStopEvents;
    vector<HANDLE> hContinueEvents;
    vector<HANDLE> hTerminateEvents;
    vector<bool> isActive;
};

inline MarkerState& getState() {
    static MarkerState state;
    return state;
}

inline void printArray() {
    MarkerState& state = getState();
    cout << "Содержимое массива: ";
    for (int i = 0; i < state.arraySize; i++) {
        cout << state.arr[i] << " ";
    }
    cout << "\n";
}

inline DWORD WINAPI markerThread(LPVOID lpParam) {
    MarkerState& state = getState();
    int id = (int)(intptr_t)lpParam;
    int idxEvent = id - 1;

    WaitForSingleObject(state.hStartEvent, INFINITE);

    srand(id);
    int markedCount = 0;
    HANDLE waitEvents[2] = { state.hTerminateEvents[idxEvent], state.hContinueEvents[idxEvent] };

    while (true) {
        int index = rand() % state.arraySize;

        EnterCriticalSection(&state.cs);
        if (state.arr[index] == 0) {
            Sleep(5);
            state.arr[index] = id;
            markedCount++;
            LeaveCriticalSection(&state.cs);
            Sleep(5);
        }
        else {
            if (!state.silent) {
                cout << "Поток marker #" << id
                    << " | Помечено элементов: " << markedCount
                    << " | Невозможный индекс: " << index << "\n";
            }
            LeaveCriticalSection(&state.cs);

            SetEvent(state.hStopEvents[idxEvent]);

            DWORD waitResult = WaitForMultipleObjects(2, waitEvents, FALSE, INFINITE);
            if (waitResult == WAIT_OBJECT_0) {
                EnterCriticalSection(&state.cs);
                for (int i = 0; i < state.arraySize; i++) {
                    if (state.arr[i] == id) {
                        state.arr[i] = 0;
                    }
                }
                LeaveCriticalSection(&state.cs);
                break;
            }
        }
    }

    return 0;
}

inline void initSystem(int size, int count, bool silentMode = false) {
    MarkerState& state = getState();
    state.arraySize = size;
    state.threadCount = count;
    state.silent = silentMode;
    state.arr = new int[size]();

    InitializeCriticalSection(&state.cs);
    state.hStartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    state.hThreads.assign(count, NULL);
    state.hStopEvents.assign(count, NULL);
    state.hContinueEvents.assign(count, NULL);
    state.hTerminateEvents.assign(count, NULL);
    state.isActive.assign(count, true);

    for (int i = 0; i < count; i++) {
        state.hStopEvents[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
        state.hContinueEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        state.hTerminateEvents[i] = CreateEvent(NULL, TRUE, FALSE, NULL);

        state.hThreads[i] = CreateThread(
            NULL,
            0,
            markerThread,
            (LPVOID)(intptr_t)(i + 1),
            0,
            NULL
        );
    }
}

inline void startMarkers() {
    SetEvent(getState().hStartEvent);
}

inline void waitAllActiveStopped() {
    MarkerState& state = getState();
    vector<HANDLE> activeStopEvents;
    for (int i = 0; i < state.threadCount; i++) {
        if (state.isActive[i]) {
            activeStopEvents.push_back(state.hStopEvents[i]);
        }
    }
    if (!activeStopEvents.empty()) {
        WaitForMultipleObjects((DWORD)activeStopEvents.size(), activeStopEvents.data(), TRUE, INFINITE);
    }
}

inline bool terminateMarker(int id) {
    MarkerState& state = getState();
    if (id < 1 || id > state.threadCount || !state.isActive[id - 1]) {
        return false;
    }

    int targetIdx = id - 1;
    SetEvent(state.hTerminateEvents[targetIdx]);
    WaitForSingleObject(state.hThreads[targetIdx], INFINITE);
    state.isActive[targetIdx] = false;
    return true;
}

inline void continueRemainingMarkers() {
    MarkerState& state = getState();
    for (int i = 0; i < state.threadCount; i++) {
        if (state.isActive[i]) {
            ResetEvent(state.hStopEvents[i]);
            SetEvent(state.hContinueEvents[i]);
        }
    }
}

inline int getActiveCount() {
    MarkerState& state = getState();
    int count = 0;
    for (bool active : state.isActive) {
        if (active) {
            count++;
        }
    }
    return count;
}

inline int countElementsWithValue(int value) {
    MarkerState& state = getState();
    int count = 0;
    for (int i = 0; i < state.arraySize; i++) {
        if (state.arr[i] == value) {
            count++;
        }
    }
    return count;
}

inline void cleanupSystem() {
    MarkerState& state = getState();
    for (int i = 0; i < state.threadCount; i++) {
        CloseHandle(state.hThreads[i]);
        CloseHandle(state.hStopEvents[i]);
        CloseHandle(state.hContinueEvents[i]);
        CloseHandle(state.hTerminateEvents[i]);
    }
    CloseHandle(state.hStartEvent);
    DeleteCriticalSection(&state.cs);
    delete[] state.arr;
    state.arr = nullptr;
    state.arraySize = 0;
    state.threadCount = 0;
    state.hThreads.clear();
    state.hStopEvents.clear();
    state.hContinueEvents.clear();
    state.hTerminateEvents.clear();
    state.isActive.clear();
}