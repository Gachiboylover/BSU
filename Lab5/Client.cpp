#include <windows.h>
#include <iostream>
#include <string>

struct Employee {
    int id;
    char name[32];
    double hours;
};

int main() {
    HANDLE hPipe = CreateFileA(
        "\\\\.\\pipe\\Lab5Pipe",
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        0,
        nullptr
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cout << "Failed to connect to server\n";
        return 1;
    }

    std::cout << "Connected to server\n";

    while (true) {
        int id;
        std::cout << "Enter employee ID (-1 to exit): ";
        std::cin >> id;

        DWORD written = 0;
        WriteFile(hPipe, &id, sizeof(id), &written, nullptr);

        if (id == -1) break;

        bool found = false;
        ReadFile(hPipe, &found, sizeof(found), &written, nullptr);

        if (!found) {
            std::cout << "Employee not found\n";
            continue;
        }

        Employee emp{};
        ReadFile(hPipe, &emp, sizeof(emp), &written, nullptr);

        std::cout << "Employee: " << emp.name << ", Hours: " << emp.hours << "\n";
        std::cout << "Enter new hours: ";
        std::cin >> emp.hours;

        WriteFile(hPipe, &emp, sizeof(emp), &written, nullptr);
    }

    CloseHandle(hPipe);
    std::cout << "Client exited\n";
    return 0;
}
