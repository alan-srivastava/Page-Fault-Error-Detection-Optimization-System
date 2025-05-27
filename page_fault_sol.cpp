#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <chrono>
#include <ctime>
#include <csignal>
#include <sys/mman.h>
#include <fcntl.h>
#include <vector>
#include <string>

bool running = true;

void signalHandler(int signum) {
    std::cout << "\nStopping monitoring via signal...\n";
    running = false;
}

// Helper to read a specific stat from /proc/self/status
std::string readStatusField(std::ifstream& statusFile, const std::string& key) {
    std::string line;
    while (getline(statusFile, line)) {
        if (line.find(key) == 0) {
            return line;
        }
    }
    return key + ": not found";
}

void logPageFaultsAndMemoryStats() {
    std::ofstream log("page_fault_solution_log.txt");
    if (!log) {
        std::cerr << "Failed to open log file.\n";
        return;
    }

    long prevMinor = -1, prevMajor = -1;
    int sameCount = 0;
    std::string line, temp;

    // Optional: Prevent future page faults by locking memory (requires sudo/root)
    // if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
    //     std::cerr << "mlockall failed: " << strerror(errno) << "\n";
    // }

    while (running) {
        // Read /proc/self/stat only once
        std::ifstream statFile("/proc/self/stat");
        if (!statFile.is_open()) break;
        getline(statFile, line);
        std::istringstream iss(line);
        for (int i = 0; i < 9; ++i) iss >> temp;

        long minorFaults = 0, majorFaults = 0;
        iss >> minorFaults >> temp >> majorFaults;
        statFile.close();

        // Reuse single ifstream to reduce mapping overhead
        std::ifstream statusFile("/proc/self/status");
        std::string vmSize = readStatusField(statusFile, "VmSize:");
        statusFile.clear();
        statusFile.seekg(0);
        std::string vmRSS = readStatusField(statusFile, "VmRSS:");
        statusFile.clear();
        statusFile.seekg(0);
        std::string vmSwap = readStatusField(statusFile, "VmSwap:");
        statusFile.close();

        // Timestamp
        std::time_t now = std::time(nullptr);
        char timeBuf[64];
        std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

        if (minorFaults == prevMinor && majorFaults == prevMajor) {
            sameCount++;
        } else {
            sameCount = 0;
        }

        prevMinor = minorFaults;
        prevMajor = majorFaults;

        // Efficient logging
        log << "[" << timeBuf << "] Minor Faults: " << minorFaults
            << " | Major Faults: " << majorFaults
            << " | Stable Count: " << sameCount << "\n"
            << "    " << vmSize << "\n"
            << "    " << vmRSS << "\n"
            << "    " << vmSwap << "\n\n";
        log.flush();

        std::cout << "Logged at " << timeBuf << " | Minor: " << minorFaults
                  << ", Major: " << majorFaults << "\n";

        if (sameCount >= 5) {
            std::cout << "No change in page faults for 5 cycles. Stopping...\n";
            break;
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    log << "Monitoring stopped due to stable page faults or signal.\n";
    log.close();
}

int main() {
    std::signal(SIGINT, signalHandler);
    std::cout << "Optimized monitoring to reduce minor page faults... Press Ctrl+C to stop.\n";
    logPageFaultsAndMemoryStats();
    return 0;
}