#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

struct employee {
    int num;
    char name[10];
    double hours;
};

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");

    if (argc != 3) {
        cerr << "Использование: Creator <имя_файла> <количество_записей>\n";
        return 1;
    }

    string filename = argv[1];
    int count = atoi(argv[2]);

    ofstream out(filename, ios::binary);
    if (!out.is_open()) {
        cerr << "Ошибка создания файла!\n";
        return 1;
    }

    for (int i = 0; i < count; ++i) {
        employee emp;
        cout << "Введите данные сотрудника " << (i + 1) << " (ID Имя Часы): ";
        cin >> emp.num >> emp.name >> emp.hours;
        out.write(reinterpret_cast<const char*>(&emp), sizeof(employee));
    }

    out.close();
    return 0;
}