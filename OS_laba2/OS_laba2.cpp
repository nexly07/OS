#include <iostream>
#include <vector>
#include <windows.h>

using namespace std;

// Тип указателя на функцию из DLL
typedef void (*ReplaceFunc)(int*, unsigned int, int, int, double);

struct ThreadData {
    vector<int> arr;
    int minVal = 0;
    int maxVal = 0;
    double average = 0.0;
};

DWORD WINAPI min_max(LPVOID lpParam) {
    ThreadData* data = static_cast<ThreadData*>(lpParam);
    const vector<int>& arr = data->arr;

    data->minVal = arr[0];
    data->maxVal = arr[0];

    for (size_t i = 1; i < arr.size(); ++i) {
        if (arr[i] < data->minVal) {
            data->minVal = arr[i];
        }
        Sleep(7);

        if (arr[i] > data->maxVal) {
            data->maxVal = arr[i];
        }
        Sleep(7);
    }

    cout << "[min_max] Минимальный элемент: " << data->minVal
        << ", максимальный элемент: " << data->maxVal << endl;

    return 0;
}

DWORD WINAPI average(LPVOID lpParam) {
    ThreadData* data = static_cast<ThreadData*>(lpParam);
    const vector<int>& arr = data->arr;

    double sum = 0.0;
    for (size_t i = 0; i < arr.size(); ++i) {
        sum += arr[i];
        Sleep(12);
    }

    data->average = sum / arr.size();

    cout << "[average] Среднее арифметическое: " << data->average << endl;

    return 0;
}

int main() {
    SetConsoleOutputCP(1251);

    int n = 0;
    cout << "Введите размерность массива: ";
    cin >> n;

    if (n <= 0) {
        cerr << "Ошибка: размерность массива должна быть больше 0." << endl;
        return 1;
    }

    ThreadData data;
    data.arr.resize(n);

    cout << "Введите элементы массива (" << n << " целых чисел): ";
    for (int i = 0; i < n; ++i) {
        cin >> data.arr[i];
    }

    HANDLE hMinMax = CreateThread(nullptr, 0, min_max, &data, 0, nullptr);
    if (hMinMax == nullptr) {
        cerr << "Ошибка создания потока min_max. Код: " << GetLastError() << endl;
        return 1;
    }

    HANDLE hAverage = CreateThread(nullptr, 0, average, &data, 0, nullptr);
    if (hAverage == nullptr) {
        cerr << "Ошибка создания потока average. Код: " << GetLastError() << endl;
        CloseHandle(hMinMax);
        return 1;
    }

    WaitForSingleObject(hMinMax, INFINITE);
    WaitForSingleObject(hAverage, INFINITE);

    CloseHandle(hMinMax);
    CloseHandle(hAverage);

    // Подключение DLL ровно в месте вызова функции
    HMODULE hLib = LoadLibraryA("test.dll");
    if (hLib == nullptr) {
        cerr << "Ошибка загрузки test.dll. Код: " << GetLastError() << endl;
        return 1;
    }

    ReplaceFunc replaceFunc = (ReplaceFunc)GetProcAddress(hLib, "replaceMinMax");
    if (replaceFunc == nullptr) {
        cerr << "Ошибка поиска функции replaceMinMax. Код: " << GetLastError() << endl;
        FreeLibrary(hLib);
        return 1;
    }

    // Вызов функции из DLL
    replaceFunc(
        data.arr.data(),
        static_cast<unsigned int>(data.arr.size()),
        data.minVal,
        data.maxVal,
        data.average
    );

    // Немедленное отключение DLL
    FreeLibrary(hLib);

    cout << "Изменённый массив: ";
    for (int elem : data.arr) {
        cout << elem << " ";
    }
    cout << endl;

    return 0;
}