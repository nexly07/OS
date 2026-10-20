#include "pch.h"
#include "../OS_laba3/marker.h"

TEST(MarkerLabTest, InitializationZerosAndWaitForStartSignal) {
    initSystem(10, 3, true);
    Sleep(30);

    MarkerState& state = getState();
    ASSERT_NE(state.arr, nullptr);
    EXPECT_EQ(state.arraySize, 10);
    EXPECT_EQ(getActiveCount(), 3);
    EXPECT_EQ(countElementsWithValue(0), 10);

    startMarkers();
    waitAllActiveStopped();
    for (int id = 1; id <= 3; id++) {
        EXPECT_TRUE(terminateMarker(id));
    }
    cleanupSystem();
}

TEST(MarkerLabTest, SingleMarkerMarksAndClearsOnTerminate) {
    initSystem(10, 1, true);
    startMarkers();
    waitAllActiveStopped();

    MarkerState& state = getState();
    int marked = countElementsWithValue(1);
    EXPECT_GT(marked, 0);
    for (int i = 0; i < state.arraySize; i++) {
        EXPECT_TRUE(state.arr[i] == 0 || state.arr[i] == 1);
    }

    EXPECT_TRUE(terminateMarker(1));
    EXPECT_EQ(getActiveCount(), 0);
    EXPECT_EQ(countElementsWithValue(1), 0);
    EXPECT_EQ(countElementsWithValue(0), 10);

    cleanupSystem();
}

TEST(MarkerLabTest, MultipleMarkersSynchronizeAndClearIndependently) {
    initSystem(15, 3, true);
    startMarkers();
    waitAllActiveStopped();

    MarkerState& state = getState();
    EXPECT_LT(countElementsWithValue(0), 15);
    for (int i = 0; i < state.arraySize; i++) {
        EXPECT_GE(state.arr[i], 0);
        EXPECT_LE(state.arr[i], 3);
    }

    EXPECT_TRUE(terminateMarker(2));
    EXPECT_EQ(getActiveCount(), 2);
    EXPECT_EQ(countElementsWithValue(2), 0);

    continueRemainingMarkers();
    waitAllActiveStopped();
    EXPECT_EQ(countElementsWithValue(2), 0);

    EXPECT_TRUE(terminateMarker(1));
    EXPECT_TRUE(terminateMarker(3));
    EXPECT_EQ(getActiveCount(), 0);
    EXPECT_EQ(countElementsWithValue(0), 15);

    cleanupSystem();
}

TEST(MarkerLabTest, InvalidAndRepeatedTerminationReturnsFalse) {
    initSystem(5, 2, true);
    startMarkers();
    waitAllActiveStopped();

    EXPECT_FALSE(terminateMarker(0));
    EXPECT_FALSE(terminateMarker(-1));
    EXPECT_FALSE(terminateMarker(3));

    EXPECT_TRUE(terminateMarker(1));
    EXPECT_FALSE(terminateMarker(1));

    continueRemainingMarkers();
    waitAllActiveStopped();
    EXPECT_TRUE(terminateMarker(2));
    EXPECT_EQ(getActiveCount(), 0);

    cleanupSystem();
}