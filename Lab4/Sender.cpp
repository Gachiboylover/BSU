#include <windows.h>
#include <iostream>
#include <string>

struct Message {
    char text[256];
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: Sender <filename>\n";
        return 1;
    }

    const char* filename = argv[1];

    HANDLE hFile = CreateFileA(
        filename,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        std::cout << "Failed to open file\n";
        return 1;
    }

    HANDLE hMutex = OpenMutexA(SYNCHRONIZE, FALSE, "Global\\Lab4_Mutex");
    HANDLE hCanWrite = OpenEventA(EVENT_MODIFY_STATE | SYNCHRONIZE, FALSE, "Global\\Lab4_CanWrite");
    HANDLE hCanRead = OpenEventA(EVENT_MODIFY_STATE | SYNCHRONIZE, FALSE, "Global\\Lab4_CanRead");

    if (!hMutex || !hCanWrite || !hCanRead) {
        std::cout << "Failed to open sync objects\n";
        return 1;
    }

    std::cout << "Sender started (type 'exit' to stop)\n";

    while (true) {
        std::string input;
        std::cout << "Enter message: ";
        std::getline(std::cin, input);

        WaitForSingleObject(hCanWrite, INFINITE);
        WaitForSingleObject(hMutex, INFINITE);

        SetFilePointer(hFile, 0, nullptr, FILE_BEGIN);

        Message msg{};
        strncpy_s(msg.text, input.c_str(), sizeof(msg.text) - 1);

        DWORD bytesWritten = 0;
        WriteFile(hFile, &msg, sizeof(msg), &bytesWritten, nullptr);

        ReleaseMutex(hMutex);
        SetEvent(hCanRead);

        if (input == "exit")
            break;
    }

    CloseHandle(hFile);
    CloseHandle(hMutex);
    CloseHandle(hCanWrite);
    CloseHandle(hCanRead);

    return 0;
}
