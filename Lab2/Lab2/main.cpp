#define NOMINMAX
#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <cmath>

static std::vector<int> g_array;
static int g_n = 0;

static int g_minValue = 0;
static int g_maxValue = 0;
static int g_minIndex = -1;
static int g_maxIndex = -1;
static double g_average = 0.0;

DWORD WINAPI MinMaxThreadProc(LPVOID lpParam);
DWORD WINAPI AverageThreadProc(LPVOID lpParam);

int inputInt(const std::string& prompt) {
    int x;
    while (true) {
        std::cout << prompt;
        if (std::cin >> x) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return x;
        }
        std::cout << "Incorrect input. Try again.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

int main() {
    while (true) {
        g_n = inputInt("Enter array size: ");
        if (g_n > 0) break;
        std::cout << "Size must be > 0\n";
    }

    g_array.resize(g_n);
    for (int i = 0; i < g_n; ++i) {
        g_array[i] = inputInt("Element[" + std::to_string(i) + "]: ");
    }

    HANDLE hMinMax = CreateThread(
        nullptr,               
        0,                     
        MinMaxThreadProc,      
        nullptr,               
        0,                     
        nullptr);              

    if (!hMinMax) {
        std::cerr << "CreateThread (min_max) failed: " << GetLastError() << "\n";
        return 1;
    }

    HANDLE hAverage = CreateThread(
        nullptr,
        0,
        AverageThreadProc,
        nullptr,
        0,
        nullptr);

    if (!hAverage) {
        std::cerr << "CreateThread (average) failed: " << GetLastError() << "\n";
        CloseHandle(hMinMax);
        return 1;
    }

    WaitForSingleObject(hMinMax, INFINITE);
    WaitForSingleObject(hAverage, INFINITE);

    CloseHandle(hMinMax);
    CloseHandle(hAverage);

    int avg_as_int = static_cast<int>(std::round(g_average));

    if (g_minIndex >= 0 && g_minIndex < g_n) {
        g_array[g_minIndex] = avg_as_int;
    }
    if (g_maxIndex >= 0 && g_maxIndex < g_n) {
        g_array[g_maxIndex] = avg_as_int;
    }

    std::cout << "\nResult array = "
        << g_average << " -> " << avg_as_int << "):\n";
    for (int i = 0; i < g_n; ++i) {
        std::cout << g_array[i] << (i + 1 < g_n ? " " : "\n");
    }

    std::cout << "Press any key to exit: ";
    std::cin.get();
    return 0;
}

DWORD WINAPI MinMaxThreadProc(LPVOID /*lpParam*/) {
    if (g_n <= 0 || g_array.empty()) {
        std::cout << "min_max: Empty aarray.\n";
        return 0;
    }

    int localMin = g_array[0];
    int localMax = g_array[0];
    int idxMin = 0;
    int idxMax = 0;

    for (int i = 1; i < g_n; ++i) {
        int val = g_array[i];
        if (val < localMin) {
            localMin = val;
            idxMin = i;
        }
        Sleep(7);

        if (val > localMax) {
            localMax = val;
            idxMax = i;
        }
        Sleep(7);
    }

    g_minValue = localMin;
    g_maxValue = localMax;
    g_minIndex = idxMin;
    g_maxIndex = idxMax;

    std::cout << "\n[min_max] Min: " << g_minValue << " (index " << g_minIndex << ")\n";
    std::cout << "[min_max] Max: " << g_maxValue << " (index " << g_maxIndex << ")\n";

    return 0;
}

DWORD WINAPI AverageThreadProc(LPVOID /*lpParam*/) {
    if (g_n <= 0 || g_array.empty()) {
        std::cout << "average: Empty array.\n";
        return 0;
    }

    long long sum = 0;
    for (int i = 0; i < g_n; ++i) {
        sum += g_array[i];
        Sleep(12);
    }

    g_average = static_cast<double>(sum) / static_cast<double>(g_n);

    std::cout << "\n[average] Avarage: " << g_average << "\n";

    return 0;
}