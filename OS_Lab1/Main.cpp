#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

using namespace std;

struct employee {
    int num;
    char name[10];
    double hours;
};

void printBinaryFile(const string& filename) {
    ifstream in(filename, ios::binary);
    if (!in) {
        cerr << "Не удалось открыть бинарный файл.\n";
        return;
    }
    cout << "\n Содержимое бинарного файла\n";
    employee emp;
    while (in.read(reinterpret_cast<char*>(&emp), sizeof(employee))) {
        cout << "ID: " << emp.num << "\tИмя: " << emp.name << "\tЧасы: " << emp.hours << "\n";
    }
    cout << "\n";
    in.close();
}

void printTextFile(const string& filename) {
    ifstream in(filename);
    if (!in) {
        cerr << "Не удалось открыть текстовый файл.\n";
        return;
    }
    cout << "\n Содержимое файла отчета\n";
    string line;
    while (getline(in, line)) {
        cout << line << "\n";
    }
    cout << "\n";
    in.close();
}

string getAppDir() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    string path(buffer);
    size_t pos = path.find_last_of("\\/");
    return (pos != string::npos) ? path.substr(0, pos + 1) : "";
}

void runProcess(const string& commandLine) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    char* cmd = new char[commandLine.length() + 1];
    strcpy(cmd, commandLine.c_str());

    if (!CreateProcessA(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        cerr << "Ошибка запуска процесса. Код ошибки: " << GetLastError() << "\n";
        delete[] cmd;
        return;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    delete[] cmd;
}

int main() {
    setlocale(LC_ALL, "Russian");

    string dir = getAppDir();
    string binFile, reportFile;
    int recordCount;
    double payRate;

    cout << "Введите имя бинарного файла: ";
    cin >> binFile;
    cout << "Введите количество записей: ";
    cin >> recordCount;

    string creatorCmd = "\"" + dir + "Creator.exe\" " + binFile + " " + to_string(recordCount);
    cout << "\n[Запуск Creator...]\n";
    runProcess(creatorCmd);

    printBinaryFile(binFile);

    cout << "Введите имя файла отчета: ";
    cin >> reportFile;
    cout << "Введите оплату за час работы: ";
    cin >> payRate;

    string reporterCmd = "\"" + dir + "Reporter.exe\" " + binFile + " " + reportFile + " " + to_string(payRate);
    cout << "\n[Запуск Reporter...]\n";
    runProcess(reporterCmd);

    printTextFile(reportFile);

    return 0;
}