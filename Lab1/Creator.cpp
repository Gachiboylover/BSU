#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>

#include "Employee.h"

template <typename T>
T validInput() {
    T value{};
    while (true) {
        std::cin >> value;
        if (std::cin.fail()) {
            std::cout << "Invalid data type, try again.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <binary_filename> <count>\n";
        return 1;
    }
    std::string filename = argv[1];
    int count = std::stoi(argv[2]);
    if (count <= 0) {
        std::cerr << "Invalid count\n";
        return 1;
    }

    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) {
        std::cerr << "Cannot open file for writing: " << filename << "\n";
        return 1;
    }

    for (int i = 0; i < count; ++i) {
        Employee e{};
        std::cout << "Enter " << (i + 1) << "/" << count << "\n";

        std::cout << "  num: ";
        e.num = validInput<int>();

        std::cout << "  name (<=9 chars): ";
        std::string tmp = validInput<std::string>();
        std::strncpy(e.name, tmp.c_str(), sizeof(e.name) - 1);

        std::cout << "  hours: ";
        e.hours = validInput<double>();

        ofs.write(reinterpret_cast<char*>(&e), sizeof(e));
    }
    std::cout << "File " << filename << " successfuly created\n";
    return 0;
}