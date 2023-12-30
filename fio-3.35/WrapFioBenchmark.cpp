#include <benchmark/benchmark.h>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <vector>
#include <fstream>
#include <unistd.h> // Include the unistd.h header for getcwd



std::string getCurrentWorkingDir() {
    char buffer[FILENAME_MAX];
    if (getcwd(buffer, FILENAME_MAX)) {
        return std::string(buffer);
    } else {
        return "";
    }
}

static std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

static void BM_WrapFio(benchmark::State& state) {
    static int iterationCount = 0; // Track the iteration count
    
    std::string currentWorkingDir = getCurrentWorkingDir(); // Get the current working directory

    // Construct full paths using the current working directory
    std::string fioPath = currentWorkingDir + "/mytest.fio";
    std::string pythonScriptPath = currentWorkingDir + "/PhoronixFio1.py";
    std::string averageValuesPath = currentWorkingDir + "/average_values.txt";


    
    // If it's the first iteration, open the file in write mode to clear previous content
    if (iterationCount = 1) {                                            //    if (iterationCount <= 1) {
        std::ofstream clearFile("run_status_output.txt", std::ios::out);
        clearFile.close();
    }
    if(iterationCount = 1) {                                            // if (iterationCount <= 1) {
        for (auto _ : state) {
            
            std::string output = exec(("fio " + fioPath).c_str());
            
            // Extract the "Run status" section
            std::string keyword = "Run status group";
            size_t startPos = output.find(keyword);
            if (startPos != std::string::npos) {
                size_t endPos = startPos;
                int consecutiveEmptyLines = 0;

                while (endPos < output.length() && consecutiveEmptyLines < 2) {
                    if (output[endPos] == '\n') {
                        if (endPos + 1 < output.length() && output[endPos + 1] == '\n') {
                            consecutiveEmptyLines++;
                        }
                    }
                    endPos++;
                }

                std::string runStatus = output.substr(startPos, endPos - startPos);
                
                // Print the "Run status" section to the console
                //if(iterationCount == 1) std::cout << runStatus << std::endl;

                // Store the "Run status" section in a .txt file in append mode
                std::ofstream outFile("run_status_output.txt", std::ios::app);
                if (outFile.is_open()) {
                    outFile << runStatus << "\n\n";
                    outFile.close(); 
                } else {
                    std::cerr << "Unable to open file for writing!" << std::endl;
                }
            } else {
                std::cerr << "Run status section not found!" << std::endl;
            }
        }

        int pythonScriptResult = 0;
        if(iterationCount == 1) pythonScriptResult = system(("sudo python3 " + pythonScriptPath).c_str());
	    //std::cout << pythonScriptResult << std::endl;
        if (pythonScriptResult != 0) {
            std::cerr << "Error running the Python script!" << std::endl;
        }

        //if(iterationCount == 1) {

        // Read the average values from the text file after executing the Python script
            int average_read = 0, average_write = 0;
            std::ifstream inFile(averageValuesPath);
            if (inFile.is_open()) {
                inFile >> average_read;
                inFile >> average_write;
                inFile.close();
	        //std::cout << average_read << std::endl;
            } else {
              std::cerr << "Unable to open file for reading!" << std::endl;
            }

        // Add the average values to the Google Benchmark user counters
            state.counters["Average Read (in MiB/s)"] = average_read;
            state.counters["Average Write (in MiB/s)"] = average_write;

            
    }
    
    iterationCount++; // Increment the iteration count after each iteration
}



//BENCHMARK(BM_WrapFio)->Iterations(1);
BENCHMARK(BM_WrapFio)->Name("fio")->Iterations(10)->Unit(benchmark::kMillisecond);


BENCHMARK_MAIN();
