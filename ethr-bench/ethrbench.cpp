#include <benchmark/benchmark.h>
#include <cstdlib>
#include <cstdio>
#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>
#include <sstream> // Include for std::istringstream
#include <string>
#include <unistd.h> // Include for getcwd

std::string getCurrentWorkingDir() {
    char buff[FILENAME_MAX]; // Create a buffer
    getcwd(buff, FILENAME_MAX);
    std::string current_working_dir(buff);
    return current_working_dir;
}


double readAverageBandwidth(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    if (getline(file, line)) {
        std::istringstream iss(line);
        std::string temp;
        double bandwidth;
        if (iss >> temp >> temp >> bandwidth) { // "Average Bandwidth: <bandwidth> bits/sec"
            return bandwidth;
        }
    }
    return 0.0; // Default value if file reading fails
}

// Function to clear the contents of a file
void clearFileContents(const std::string& filename) {
    std::ofstream file(filename, std::ofstream::out | std::ofstream::trunc);
    file.close();
}

// Function to start the Ethr server as a separate process

void startEthrServer() {
    std::string command = (getCurrentWorkingDir() + "/ethr -s > /dev/null &").c_str();
    std::system(command.c_str());
    std::this_thread::sleep_for(std::chrono::seconds(5));
}

void runEthrBenchmark() {
    std::string command = (getCurrentWorkingDir() + "/ethr -c localhost -t b -p tcp -o ethroutput.txt > /dev/null").c_str();
    std::system(command.c_str());
}



// Benchmark your Ethr test case
static void BM_EthrBenchmark(benchmark::State& state) {
    // Clear the contents of the file before starting the benchmark
    clearFileContents("ethroutput.txt");

    startEthrServer();

    for (auto _ : state) {
        runEthrBenchmark();
        
    }

    // Stop the Ethr server after the benchmark is completed
    std::system("killall ethr");

    

    //state.SetItemsProcessed(state.iterations());

    // Run the Python script
    std::system("python3 ethr-average.py");

    double avgBandwidth = readAverageBandwidth(getCurrentWorkingDir() + "/ethres.txt");

    // Convert the average bandwidth to bits/second
    //double avgBandwidthBitsPerSecond = avgBandwidth * 1e6; // 1 Mbps = 1,000,000 bits

   
    // Set the average bandwidth as a user-defined counter
// Set the average bandwidth as a user-defined counter
    state.counters["AverageBandwidth (bits/sec)"] = avgBandwidth;

    
}

BENCHMARK(BM_EthrBenchmark)->Iterations(2)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
