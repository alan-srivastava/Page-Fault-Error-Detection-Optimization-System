#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <ctime>
#include <csignal>
#include <vector>
#include <cstring>  // memset
#include <fcntl.h>
#include <sys/mman.h>

bool running = true;

void signalHandler(int signum) {
    std::cout << "\nStopping monitoring via signal...\n";
    running = false;
}

// Simulate stack growth
void growStack(int depth) {
    if (depth <= 0) return;
    char largeStackBuffer[4096];  // Allocate 4KB
    memset(largeStackBuffer, 0, sizeof(largeStackBuffer));
    growStack(depth - 1);
}

// Simulate heap allocation and access
void allocateHeapPages(std::vector<char*>& heapBlocks) {
    for (int i = 0; i < 100; ++i) {
        char* block = new char[4096]; // 4KB each
        block[0] = i; // Trigger fault
        heapBlocks.push_back(block);
    }
}

// Simulate thread creation
void createThreads() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([]() {
            char localBuf[4096];
            memset(localBuf, 1, sizeof(localBuf));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        });
    }
    for (auto& t : threads) t.join();
}

// Simulate memory-mapped I/O
void triggerIOBufferFault() {
    int fd = open("/etc/hostname", O_RDONLY);
    if (fd == -1) return;
    char* mapped = (char*)mmap(nullptr, 4096, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped != MAP_FAILED) {
        char c = mapped[0]; // Trigger fault
        (void)c;
        munmap(mapped, 4096);
    }
    close(fd);
}

// Logging and monitoring logic
void logPageFaultsAndMemoryStats() {
    std::ofstream log("page_fault_test_log.txt");
    if (!log) {
        std::cerr << "Failed to open log file.\n";
        return;
    }

    long prevMinor = -1, prevMajor = -1;
    int sameCount = 0;
    int cycleCount = 0;
    const int maxCycles = 10;
    std::vector<char*> heapBlocks;

    while (running && cycleCount < maxCycles) {
        cycleCount++;

        std::ifstream statFile("/proc/self/stat");
        std::string line;
        getline(statFile, line);
        std::istringstream iss(line);

        std::string temp;
        long minorFaults = 0;
        long majorFaults = 0;

        for (int i = 0; i < 9; ++i) iss >> temp;
        iss >> minorFaults;
        iss >> temp;
        iss >> majorFaults;

        std::ifstream statusFile("/proc/self/status");
        std::string memLine;
        std::string vmSize, vmRSS, vmSwap;
        while (getline(statusFile, memLine)) {
            if (memLine.find("VmSize:") != std::string::npos) vmSize = memLine;
            if (memLine.find("VmRSS:") != std::string::npos) vmRSS = memLine;
            if (memLine.find("VmSwap:") != std::string::npos) vmSwap = memLine;
        }

        std::time_t now = std::time(nullptr);
        char timeBuf[100];
        std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

        if (minorFaults == prevMinor && majorFaults == prevMajor) {
            sameCount++;
        } else {
            sameCount = 0;
        }

        prevMinor = minorFaults;
        prevMajor = majorFaults;

        log << "[" << timeBuf << "] Minor Faults: " << minorFaults
            << " | Major Faults: " << majorFaults
            << " | Stable Count: " << sameCount
            << " | Cycle: " << cycleCount << "/" << maxCycles << "\n";
        log << "    " << vmSize << "\n";
        log << "    " << vmRSS << "\n";
        log << "    " << vmSwap << "\n\n";
        log.flush();

        std::cout << "Logged at " << timeBuf << " | Minor: " << minorFaults
                  << ", Major: " << majorFaults
                  << " | Cycle " << cycleCount << "\n";

        // Trigger scenarios causing minor page faults
        growStack(10);
        allocateHeapPages(heapBlocks);
        createThreads();
        triggerIOBufferFault();

        if (sameCount >= 5) {
            std::cout << "No change in page faults for 5 consecutive cycles. Terminating...\n";
            break;
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    for (char* ptr : heapBlocks) delete[] ptr;
    log << "Monitoring stopped due to stable page faults or signal.\n";
    log.close();
}

int main() {
    std::signal(SIGINT, signalHandler);
    std::cout << "Simulating and monitoring page faults on Ubuntu...\nPress Ctrl+C to stop early.\n";
    logPageFaultsAndMemoryStats();
    return 0;
}