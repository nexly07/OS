#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>

using namespace std;

struct employee {
    int num;
    char name[10];
    double hours;
};

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");

    if (argc != 4) {
        cerr << "Использование: Reporter <исходный_файл> <файл_отчета> <оплата_за_час>\n";
        return 1;
    }

    string bin_filename = argv[1];
    string txt_filename = argv[2];
    double pay_rate = atof(argv[3]);

    ifstream in(bin_filename, ios::binary);
    if (!in.is_open()) {
        cerr << "Ошибка открытия бинарного файла!\n";
        return 1;
    }

    ofstream out(txt_filename);
    if (!out.is_open()) {
        cerr << "Ошибка создания текстового файла!\n";
        return 1;
    }

    out << "Отчет по файлу \"" << bin_filename << "\"\n";
    out << "Номер сотрудника, имя сотрудника, часы, зарплата\n";

    employee emp;
    while (in.read(reinterpret_cast<char*>(&emp), sizeof(employee))) {
        double salary = emp.hours * pay_rate;
        out << emp.num << ", " << emp.name << ", " << emp.hours << ", " << salary << "\n";
    }

    in.close();
    out.close();
    return 0;
}