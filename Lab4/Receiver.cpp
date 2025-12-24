#include <windows.h>
#include <iostream>
#include <string>

struct Message {
    char text[256];
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: Receiver <filename>\n";
        return 1;
    }

    const char* filename = argv[1];

    HANDLE hFile = CreateFileA(
        filename,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        std::cout << "Failed to create file\n";
        return 1;
    }

    HANDLE hMutex = CreateMutexA(nullptr, FALSE, "Global\\Lab4_Mutex");
    HANDLE hCanWrite = CreateEventA(nullptr, FALSE, TRUE, "Global\\Lab4_CanWrite");
    HANDLE hCanRead  = CreateEventA(nullptr, FALSE, FALSE, "Global\\Lab4_CanRead");

    if (!hMutex || !hCanWrite || !hCanRead) {
        std::cout << "Failed to create sync objects\n";
        return 1;
    }

    std::cout << "Receiver started\n";

    while (true) {
        WaitForSingleObject(hCanRead, INFINITE);
        WaitForSingleObject(hMutex, INFINITE);

        SetFilePointer(hFile, 0, nullptr, FILE_BEGIN);

        Message msg{};
        DWORD bytesRead = 0;
        ReadFile(hFile, &msg, sizeof(msg), &bytesRead, nullptr);

        ReleaseMutex(hMutex);
        SetEvent(hCanWrite);

        if (strcmp(msg.text, "exit") == 0) {
            std::cout << "Exit command received\n";
            break;
        }

        std::cout << "Received: " << msg.text << "\n";
    }

    CloseHandle(hFile);
    CloseHandle(hMutex);
    CloseHandle(hCanWrite);
    CloseHandle(hCanRead);

    return 0;
}
