#include <benchmark/benchmark.h>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include<iostream>
#include <unistd.h> // Include for getcwd

std::string getCurrentWorkingDir() {
    char buffer[FILENAME_MAX];
    if (getcwd(buffer, FILENAME_MAX)) {
        return std::string(buffer);
    } else {
        return "";
    }
}

double extractAverage(const std::string& filename, const std::string& searchStr) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return -1.0;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.find(searchStr) != std::string::npos) {
            std::istringstream iss(line);
            std::string temp;
            double value;
            iss >> temp >> temp >> value; // Skip the first two strings and read the number
            if (iss.fail()) {
                std::cerr << "Error converting string to double: " << line << std::endl;
                return -1.0;
            }
            return value;
        }
    }
    return -1.0; // Indicates error or value not found
}


// Benchmark function for integer memory
static void Benchmark_RAMSMP_INTmem(benchmark::State& state) {
    double total_average = 0.0;
    int num_iterations = 0;
    
    std::string currentWorkingDir = getCurrentWorkingDir(); // Get the current working directory

    // Construct full paths using the current working directory
    std::string ramsmpPath = currentWorkingDir + "/ramsmp";
    std::string pythonScriptPath = currentWorkingDir + "/average.py";
    std::string averageValuesPath = currentWorkingDir + "/output.txt";

    // Clear the contents of output.txt before starting the benchmark
    std::ofstream clearFile(averageValuesPath);
    clearFile.close();

    for (auto _ : state) {
        // Append to output.txt and add a separator line to indicate start of new iteration
        std::system(("echo '=== ITERATION START ===' >> " + averageValuesPath).c_str());
        std::system((ramsmpPath + " -b 3 >> " + averageValuesPath).c_str());  // Append output to the file

        double average_value = extractAverage(averageValuesPath, "INTEGER   AVERAGE:");
        if (average_value > 0) {
            total_average += average_value;
            ++num_iterations;
        }
    }

    // Run the Python script
    std::system(("python3 " + pythonScriptPath).c_str());

    // Read the result from result.txt
    double python_result = -1.0;
    std::ifstream resultFile(currentWorkingDir + "/result.txt");
    
    std::string line;
    if (std::getline(resultFile, line)) {
        // Assuming the line has the format "Overall average (MB/S): VALUE", extracting the VALUE.
        python_result = std::stod(line.substr(line.find_last_of(' ') + 1));
    }

    // Only set the PYTHON_AVERAGE counter
    state.counters["Average_integer_mem (in MiB/s)"] = benchmark::Counter(python_result);
}

// Benchmark function for float memory
static void Benchmark_RAMSMP_FLOATmem(benchmark::State& state) {
    double total_average = 0.0;
    int num_iterations = 0;

    std::string currentWorkingDir = getCurrentWorkingDir(); // Get the current working directory

    // Construct full paths using the current working directory
    std::string ramsmpPath = currentWorkingDir + "/ramsmp";
    std::string pythonScriptPath = currentWorkingDir + "/avg.py";
    std::string averageValuesPath = currentWorkingDir + "/out.txt";

    // Clear the contents of out.txt before starting the benchmark
    std::ofstream clearFile(averageValuesPath);
    clearFile.close();

    for (auto _ : state) {
        // Append to out.txt and add a separator line to indicate start of new iteration
        std::system(("echo '=== ITERATION START ===' >> " + averageValuesPath).c_str());
        std::system((ramsmpPath + " -b 6 >> " + averageValuesPath).c_str());  // Append output to the file

        double average_value = extractAverage(averageValuesPath, "FL-POINT   AVERAGE:");
        if (average_value > 0) {
            total_average += average_value;
            ++num_iterations;
        }
    }

    // Run the Python script
    std::system(("python3 " + pythonScriptPath).c_str());

    // Read the result from res.txt
    double python_res = -1.0;
    std::ifstream resultFile(currentWorkingDir + "/res.txt");
    
    std::string line;
    if (std::getline(resultFile, line)) {
        // Assuming the line has the format "Overall average (MB/S): VALUE", extracting the VALUE.
        python_res = std::stod(line.substr(line.find_last_of(' ') + 1));
    }

    // Only set the PYTHON_AVERAGE counter
    state.counters["Average_Float_mem (in MiB/s)"] = benchmark::Counter(python_res);
}

// Register both benchmarks to run
BENCHMARK(Benchmark_RAMSMP_INTmem)->Iterations(10)->Unit(benchmark::kMillisecond);
BENCHMARK(Benchmark_RAMSMP_FLOATmem)->Iterations(10)->Unit(benchmark::kMillisecond);

// Only one main function, as provided by the benchmark library
BENCHMARK_MAIN();
