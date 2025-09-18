#define NOMINMAX
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>

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

void runAndWait(const std::string& cmd) {
    STARTUPINFOA si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);

    char cmdline[1024];
    strncpy(cmdline, cmd.c_str(), sizeof(cmdline) - 1);
    cmdline[sizeof(cmdline) - 1] = 0;

    if (!CreateProcessA(nullptr, cmdline, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
        std::cerr << "CreateProcess failed (" << GetLastError() << ")\n";
        return;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void printBinaryFile(const std::string& filename) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) {
        std::cerr << "Failed to open  " << filename << "\n";
        return;
    }
    Employee e;
    std::cout << "Binary file:\n";
    while (ifs.read(reinterpret_cast<char*>(&e), sizeof(e))) {
        std::cout << e.num << " " << e.name << " " << e.hours << "\n";
    }
}

void printTextFile(const std::string& filename) {
    std::ifstream ifs(filename);
    if (!ifs) {
        std::cerr << "Failed to open  " << filename << "\n";
        return;
    }
    std::cout << "Text file:\n";
    std::string line;
    while (std::getline(ifs, line)) {
        std::cout << line << "\n";
    }
}

int main() {
    std::cout << "Enter name of the binary file: ";
    std::string binfile = validInput<std::string>();
    std::cout << "Enter number of records: ";
    int count = validInput<int>();

    // Show Creator
    std::string cmd1 = "Creator.exe \"" + binfile + "\" " + std::to_string(count);
    runAndWait(cmd1);

    // Show bin
    printBinaryFile(binfile);

    // start Reporter
    std::cout << "Enter name of the reporter file: ";
    std::string repfile = validInput<std::string>();
    std::cout << "Enter rate pet our: ";
    double rate = validInput<double>();

    std::string cmd2 = "Reporter.exe \"" + binfile + "\" \"" + repfile + "\" " + std::to_string(rate);
    runAndWait(cmd2);

    // show report
    printTextFile(repfile);

    return 0;
}