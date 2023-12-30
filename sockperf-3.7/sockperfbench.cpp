#include <benchmark/benchmark.h>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>

// ... [Other functions like getCurrentWorkingDir]

std::string getCurrentWorkingDir() {
    char buffer[FILENAME_MAX];
    if (getcwd(buffer, sizeof(buffer))) {
        return std::string(buffer);
    } else {
        return "";
    }
}

// read average results from python result 

double readAveragelatency(const std::string& filePath) {
    std::ifstream infile(filePath);
    double latency = 0.0;
    if (!infile) {
        std::cerr << "Unable to open file: " << filePath << std::endl;
        return -1; // Return a negative value to indicate failure
    }

    std::string line;
    if (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::string temp;
        while (iss >> temp) {
            if (std::stringstream(temp) >> latency) {
                break; // Break if a number is successfully read
            }
        }
    } else {
        std::cerr << "Failed to read from file: " << filePath << std::endl;
        return -1;
    }

    std::cout << "Read latency from file: " << latency << " usec" << std::endl; // Debugging output
    infile.close();
    return latency;
}

void startSockperfServer() {
    std::string cwd = getCurrentWorkingDir();
    std::string command = cwd + "/sockperf server --tcp --port 12345 &";
    std::system(command.c_str());
    std::this_thread::sleep_for(std::chrono::seconds(5)); // Wait for server to start
}


void clearFileContents() {
    std::ofstream ofs;
    ofs.open("sockout.txt", std::ofstream::out | std::ofstream::trunc);
    ofs.close();
}

void runSockperfLatencyTest() {
    std::string cwd = getCurrentWorkingDir();
    std::string command = cwd + "/sockperf under-load -p 12345 -i 127.0.0.1 --tcp -m 64 -t 20 --pps 1000 >> sockout.txt";
    std::system(command.c_str());
}


static void BM_SockperfLatencyUnderLoad(benchmark::State& state) {
    startSockperfServer();
    clearFileContents(); // Clear the file at the start of each benchmark run

    for (auto _ : state) {
        runSockperfLatencyTest();
    }

    // Stop the sockperf server after the benchmark is completed
    std::system("killall sockperf");

    // Run the Python script
    std::system("python3 sock-avg.py");

    std::string cwd = getCurrentWorkingDir();
    double avglatency = readAveragelatency(cwd + "/sockres.txt");
    // Set the average bandwidth as a user-defined counter
    state.counters["AverageLatency"] = avglatency;
}

BENCHMARK(BM_SockperfLatencyUnderLoad)->Iterations(2)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();

