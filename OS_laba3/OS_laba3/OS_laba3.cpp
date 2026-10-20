#include "marker.h"
#include <clocale>

int main() {
    setlocale(LC_ALL, "Russian");

    int size = 0;
    cout << "Введите размерность массива: ";
    cin >> size;
    if (size <= 0) {
        cout << "Размерность массива должна быть больше 0.\n";
        return 1;
    }

    int threadCount = 0;
    cout << "Введите количество потоков marker: ";
    cin >> threadCount;
    if (threadCount <= 0) {
        cout << "Количество потоков должно быть больше 0.\n";
        return 1;
    }

    initSystem(size, threadCount);
    startMarkers();

    while (getActiveCount() > 0) {
        waitAllActiveStopped();
        printArray();

        int terminateId = 0;
        while (true) {
            cout << "Введите порядковый номер потока marker для завершения (1-" << threadCount << "): ";
            cin >> terminateId;
            if (terminateMarker(terminateId)) {
                break;
            }
            cout << "Некорректный номер или поток уже завершен. Попробуйте снова.\n";
        }

        printArray();
        continueRemainingMarkers();
    }

    cleanupSystem();
    return 0;
}