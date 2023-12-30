#include <benchmark/benchmark.h>
#include <fstream>
#include <string>
#include <regex>
#include <cstdlib>
#include <vector>
#include <numeric>
#include <thread>
#include <chrono>
#include <unistd.h>

std::string getCurrentWorkingDir() {
    char buff[FILENAME_MAX];
    getcwd(buff, FILENAME_MAX);
    std::string current_working_dir(buff);
    return current_working_dir;
}

class Iperf3Benchmark : public benchmark::Fixture {
public:
    std::string iperfPath; // Add a member variable for iperf3 path

    Iperf3Benchmark() {
        // Set the iperf3 path in the constructor using getCurrentWorkingDir()
        iperfPath = getCurrentWorkingDir() + "/iperf3";
    }

    void SetUp(const ::benchmark::State& state) override {
        std::ofstream ofs("iperf_results.txt", std::ofstream::out | std::ofstream::trunc);
        ofs.close();

        std::string iperfServerCommand = iperfPath + " -s > /dev/null 2>&1 &";
        std::system(iperfServerCommand.c_str());
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    void TearDown(const ::benchmark::State& state) override {
        std::string iperfStopCommand = "pkill iperf3";
        std::system(iperfStopCommand.c_str());
    }

    double parseAverageBitrate(const std::string& filename, const std::string& type) {
        std::ifstream file(filename);
        std::string line;
        std::regex regex_sum("\\[SUM\\].*?([0-9.]+) Mbits/sec.*" + type);
        std::smatch match;
        std::vector<double> bitrates;

        while (std::getline(file, line)) {
            if (std::regex_search(line, match, regex_sum) && match.size() > 1) {
                bitrates.push_back(std::stod(match[1]));
            }
        }

        double sum = std::accumulate(bitrates.begin(), bitrates.end(), 0.0);
        return bitrates.empty() ? 0.0 : sum / bitrates.size();
    }

    void RunIperfClient(const std::string& resultsFile) {
        std::string iperfClientCommand = iperfPath +
            " -c localhost -p 5201 -t 10 -i 5 -u -b 100M -P 4 -R >> " +
            resultsFile;
        std::system(iperfClientCommand.c_str());
    }
};

BENCHMARK_DEFINE_F(Iperf3Benchmark, Client)(benchmark::State& state) {
    for (auto _ : state) {
        // Append the results of each iteration using the RunIperfClient method
        RunIperfClient("iperf_results.txt");

        // Calculate average bitrates
        double avgSenderBitrate = parseAverageBitrate("iperf_results.txt", "sender");
        double avgReceiverBitrate = parseAverageBitrate("iperf_results.txt", "receiver");

        // Set user counters
        state.counters["Average Sender Bitrate (Mbits/sec)"] = avgSenderBitrate;
        state.counters["Average Receiver Bitrate (Mbits/sec)"] = avgReceiverBitrate;
    }
}

BENCHMARK_REGISTER_F(Iperf3Benchmark, Client)->Iterations(3)->Unit(benchmark::kMillisecond)->Name("iperf");

BENCHMARK_MAIN();
