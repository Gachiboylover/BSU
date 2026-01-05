#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

struct Employee {
    int id;
    char name[32];
    double hours;
};

int main() {
    const char* fileName = "employees.bin";

    {
        std::ofstream init(fileName, std::ios::binary | std::ios::trunc);
        Employee emps[] = {
            {1, "Alice", 40.0},
            {2, "Bob", 35.5},
            {3, "Charlie", 20.0}
        };
        init.write(reinterpret_cast<char*>(emps), sizeof(emps));
    }

    HANDLE hPipe = CreateNamedPipeA(
        "\\\\.\\pipe\\Lab5Pipe",
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        1,
        1024,
        1024,
        0,
        nullptr
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cout << "Failed to create named pipe\n";
        return 1;
    }

    std::cout << "Server waiting for client...\n";
    ConnectNamedPipe(hPipe, nullptr);

    while (true) {
        int id = 0;
        DWORD bytes = 0;

        if (!ReadFile(hPipe, &id, sizeof(id), &bytes, nullptr))
            break;

        if (id == -1)
            break;

        std::fstream file(fileName, std::ios::binary | std::ios::in | std::ios::out);
        if (!file) {
            std::cout << "File open error\n";
            break;
        }

        Employee emp{};
        bool found = false;
        std::streampos pos;

        while (file.read(reinterpret_cast<char*>(&emp), sizeof(emp))) {
            if (emp.id == id) {
                pos = file.tellg() - static_cast<std::streamoff>(sizeof(emp));
                found = true;
                break;
            }
        }

        WriteFile(hPipe, &found, sizeof(found), &bytes, nullptr);

        if (!found) {
            file.close();
            continue;
        }

        WriteFile(hPipe, &emp, sizeof(emp), &bytes, nullptr);

        ReadFile(hPipe, &emp, sizeof(emp), &bytes, nullptr);

        file.seekp(pos);
        file.write(reinterpret_cast<char*>(&emp), sizeof(emp));
        file.close();

        std::cout << "Employee updated: ID " << emp.id << "\n";
    }

    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);

    std::cout << "Server stopped\n";
    return 0;
}
