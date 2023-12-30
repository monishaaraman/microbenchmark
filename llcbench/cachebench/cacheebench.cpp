#include <iostream>  // Add this line
#include <benchmark/benchmark.h>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <unistd.h> // for getcwd
#include <limits.h> // for PATH_MAX
// ... rest of my code ...

std::string getCurrentWorkingDirectory() {
    char temp[PATH_MAX];
    if (getcwd(temp, sizeof(temp)) != nullptr) {
        return std::string(temp);
    } else {
        // Handle error, could use errno to find out what went wrong
        return std::string("");
    }
}

double readAverageFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return -1.0;
    }

    std::string line;
    if (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string temp;
        double average;

        // Skip the first three words ("Overall average (MB/s):") and then read the average
        if (iss >> temp >> temp >> temp >> average) {
            return average;
        } else {
            std::cerr << "Error parsing line: " << line << std::endl;
        }
    } else {
        std::cerr << "No data in file: " << filename << std::endl;
    }
    return -1.0;
}


static void BM_CacheBench(benchmark::State& state) {
    // Clear the contents of cacheoutput.txt before starting the benchmark
    std::ofstream clearFile("cacheoutput.txt");
    clearFile.close();

    std::string cwd = getCurrentWorkingDirectory();
    std::string cachebenchCommand = cwd + "/cachebench -b -m 16 -e 1 -x 2 -d 5";

    for (auto _ : state) {
        std::system((cachebenchCommand + " >> cacheoutput.txt 2>&1").c_str());
    }

    // Run the Python script after the benchmark
    std::system("python3 average-cache.py");

    // Read the average from cacheresults.txt
    double averageMBs = readAverageFromFile("cacheresults.txt");

    // Set the average as a user-defined counter
    state.counters["Average_Cache (in MB/s)"] = benchmark::Counter(averageMBs);

    // Optionally, read back results if your Python script writes to a file
    // Example: Reading from 'results.txt'
    std::ifstream resultFile("cacheresults.txt");
    std::string line;
    if (std::getline(resultFile, line)) {
        // Process the line as needed
    }
}

BENCHMARK(BM_CacheBench)->Iterations(3)->Unit(benchmark::kMillisecond);
BENCHMARK_MAIN();
