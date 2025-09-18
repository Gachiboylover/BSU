#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>

#include "Employee.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <binary_filename> <report_filename> <rate>\n";
        return 1;
    }
    std::string binfile = argv[1];
    std::string repfile = argv[2];
    double rate = std::stod(argv[3]);

    std::ifstream ifs(binfile, std::ios::binary);
    if (!ifs) {
        std::cerr << "Cannot open file: " << binfile << "\n";
        return 1;
    }

    std::vector<Employee> emps;
    Employee e;
    while (ifs.read(reinterpret_cast<char*>(&e), sizeof(e))) {
        emps.push_back(e);
    }

    std::sort(emps.begin(), emps.end(), [](const Employee& a, const Employee& b) {
        return a.num < b.num;
        });

    std::ofstream ofs(repfile);
    if (!ofs) {
        std::cerr << "Cannot open report file: " << repfile << "\n";
        return 1;
    }

    ofs << "Report \"" << binfile << "\"\n";
    ofs << std::setw(6) << "Number" << " "
        << std::setw(10) << "Name" << " "
        << std::setw(8) << "Hours" << " "
        << std::setw(12) << "Salary" << "\n";

    ofs << std::fixed << std::setprecision(2);
    for (auto& emp : emps) {
        double salary = emp.hours * rate;
        ofs << std::setw(6) << emp.num << " "
            << std::setw(10) << emp.name << " "
            << std::setw(8) << emp.hours << " "
            << std::setw(12) << salary << "\n";
    }

    std::cout << "Report is saved to " << repfile << "\n";
    return 0;
}