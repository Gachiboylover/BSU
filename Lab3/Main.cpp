#define NOMINMAX
#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <random>
#include <chrono>
#include <algorithm>

constexpr DWORD SLEEP_MARK = 5;
constexpr size_t MAX_THREADS = MAXIMUM_WAIT_OBJECTS;

static std::vector<int> g_array;
static CRITICAL_SECTION g_cs;
static CRITICAL_SECTION g_printCS;


static HANDLE hStartEvent = nullptr;
static std::vector<HANDLE> g_blockedEvents;
static std::vector<HANDLE> g_continueEvents;
static std::vector<HANDLE> g_stopEvents;
static std::vector<HANDLE> g_threadHandles;
static std::vector<bool> g_threadActive;

static std::vector<std::vector<int>> g_markedIndices;

int inputInt(const std::string& prompt, int minVal = INT_MIN, int maxVal = INT_MAX) {
    int x;
    while (true) {
        std::cout << prompt;
        if (std::cin >> x) {
            if (x < minVal || x > maxVal) {
                std::cout << "Value must be between [" << minVal << ", " << maxVal << "]\n";
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return x;
        }
        else {
            std::cout << "Invalid input, try again.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

void printArray() {
    EnterCriticalSection(&g_cs);
    std::cout << "Array: ";
    for (size_t i = 0; i < g_array.size(); ++i) {
        std::cout << g_array[i] << (i + 1 < g_array.size() ? " " : "\n");
    }
    LeaveCriticalSection(&g_cs);
}

DWORD WINAPI MarkerThreadProc(LPVOID lpParam) {
    int id = static_cast<int>(reinterpret_cast<intptr_t>(lpParam));
    size_t size = g_array.size();

    srand(id);

    while (true) {
        int r = rand();
        int idx = r % static_cast<int>(size);

        EnterCriticalSection(&g_cs);
        if (g_array[idx] == 0) {
            LeaveCriticalSection(&g_cs);
            Sleep(SLEEP_MARK);
            EnterCriticalSection(&g_cs);
            if (g_array[idx] == 0) {
                g_array[idx] = id;
                g_markedIndices[id - 1].push_back(idx);
                LeaveCriticalSection(&g_cs);
                Sleep(SLEEP_MARK);
                continue;
            }
            else {
                LeaveCriticalSection(&g_cs);
            }
        }
        else {
            LeaveCriticalSection(&g_cs);
        }

        EnterCriticalSection(&g_cs);
        int markedCount = static_cast<int>(g_markedIndices[id - 1].size());
        LeaveCriticalSection(&g_cs);

        EnterCriticalSection(&g_printCS);
        std::cout << "[marker " << id << "] Cannot mark index " << idx
            << ". Marked elements: " << markedCount << "\n";
        LeaveCriticalSection(&g_printCS);


        SetEvent(g_blockedEvents[id - 1]);

        HANDLE waitHandles[2] = { g_continueEvents[id - 1], g_stopEvents[id - 1] };
        DWORD w = WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);

        if (w == WAIT_OBJECT_0) {
            continue;
        }
        else if (w == WAIT_OBJECT_0 + 1) {
            EnterCriticalSection(&g_cs);
            for (int pos : g_markedIndices[id - 1]) {
                if (pos >= 0 && static_cast<size_t>(pos) < g_array.size()) {
                    if (g_array[pos] == id) g_array[pos] = 0;
                }
            }
            LeaveCriticalSection(&g_cs);

            EnterCriticalSection(&g_printCS);
            std::cout << "[marker " << id << "] Finishing. Cleared "
                << g_markedIndices[id - 1].size() << " marks.\n";
            LeaveCriticalSection(&g_printCS);

            return 0;
        }
        else {
            std::cerr << "[marker " << id << "] Wait error: " << GetLastError() << "\n";
            return 1;
        }
    }
    return 0;
}

int main() {
    InitializeCriticalSection(&g_printCS);
    InitializeCriticalSection(&g_cs);

    std::cout << "=== Laboratory Work 3 ===\n";

    int n = inputInt("Enter array size: ", 1, INT_MAX);
    g_array.assign(static_cast<size_t>(n), 0);

    int m = inputInt("Enter number of marker threads (1.." + std::to_string(MAX_THREADS) + "): ", 1, static_cast<int>(MAX_THREADS));

    g_blockedEvents.assign(m, nullptr);
    g_continueEvents.assign(m, nullptr);
    g_stopEvents.assign(m, nullptr);
    g_threadHandles.assign(m, nullptr);
    g_threadActive.assign(m, true);
    g_markedIndices.assign(m, std::vector<int>());

    hStartEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!hStartEvent) {
        std::cerr << "Failed to create start event\n";
        DeleteCriticalSection(&g_cs);
        return 1;
    }

    for (int i = 0; i < m; ++i) {
        g_blockedEvents[i] = CreateEvent(nullptr, TRUE, FALSE, nullptr);
        g_continueEvents[i] = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        g_stopEvents[i] = CreateEvent(nullptr, FALSE, FALSE, nullptr);

        if (!g_blockedEvents[i] || !g_continueEvents[i] || !g_stopEvents[i]) {
            std::cerr << "Failed to create event\n";
            return 1;
        }
    }

    for (int i = 0; i < m; ++i) {
        intptr_t id = static_cast<intptr_t>(i + 1);
        HANDLE h = CreateThread(nullptr, 0, MarkerThreadProc, reinterpret_cast<LPVOID>(id), 0, nullptr);
        if (!h) {
            std::cerr << "Failed to start thread\n";
            g_threadActive[i] = false;
        }
        else {
            g_threadHandles[i] = h;
            g_threadActive[i] = true;
        }
    }

    SetEvent(hStartEvent);

    int activeCount = m;
    while (activeCount > 0) {
        std::vector<HANDLE> waitBlocked;
        for (int i = 0; i < m; ++i)
            if (g_threadActive[i]) waitBlocked.push_back(g_blockedEvents[i]);

        if (waitBlocked.empty()) break;

        DWORD waitRes = WaitForMultipleObjects(
            static_cast<DWORD>(waitBlocked.size()),
            waitBlocked.data(),
            TRUE,
            INFINITE
        );

        if (waitRes == WAIT_FAILED) {
            std::cerr << "Wait error\n";
            break;
        }

        std::cout << "\n--- All active threads are blocked ---\n";
        printArray();

        int chosen = -1;
        while (true) {
            chosen = inputInt("Enter marker thread number to stop: ", 1, m);
            if (!g_threadActive[chosen - 1]) {
                std::cout << "This thread is already stopped.\n";
                continue;
            }
            break;
        }

        SetEvent(g_stopEvents[chosen - 1]);

        if (g_threadHandles[chosen - 1]) {
            WaitForSingleObject(g_threadHandles[chosen - 1], INFINITE);
            CloseHandle(g_threadHandles[chosen - 1]);
            g_threadHandles[chosen - 1] = nullptr;
        }
        g_threadActive[chosen - 1] = false;
        --activeCount;

        ResetEvent(g_blockedEvents[chosen - 1]);

        std::cout << "\n--- After stopping thread " << chosen << " ---\n";
        printArray();

        for (int i = 0; i < m; ++i) {
            if (g_threadActive[i]) {
                ResetEvent(g_blockedEvents[i]);
                SetEvent(g_continueEvents[i]);
            }
        }
    }

    std::cout << "\nAll threads finished. Final array:\n";
    printArray();

    for (int i = 0; i < m; ++i) {
        if (g_blockedEvents[i]) CloseHandle(g_blockedEvents[i]);
        if (g_continueEvents[i]) CloseHandle(g_continueEvents[i]);
        if (g_stopEvents[i]) CloseHandle(g_stopEvents[i]);
        if (g_threadHandles[i]) {
            WaitForSingleObject(g_threadHandles[i], INFINITE);
            CloseHandle(g_threadHandles[i]);
        }
    }

    if (hStartEvent) CloseHandle(hStartEvent);
    DeleteCriticalSection(&g_cs);
    DeleteCriticalSection(&g_printCS);


    std::cout << "Press Enter to exit...";
    std::cin.get();
    return 0;
}
