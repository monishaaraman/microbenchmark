#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <regex>
#include <cstdio> // for freopen
#include <pqxx/pqxx>



std::string getSystemModelName() {
    std::ifstream file("/sys/class/dmi/id/product_name");
    if (file.is_open()) {
        std::string modelName;
        std::getline(file, modelName);
        return modelName;
    } else {
        return "Error: Unable to open /sys/class/dmi/id/product_name";
    }
}

   

std::string getFirstToken(const std::string &s, char delimiter) {
    std::istringstream tokenStream(s);
    std::string token;
    
    if (std::getline(tokenStream, token, delimiter)) {
        return token;
    }

    // Return an empty string if there are no tokens
    return "";
}


//Process Disk console Data 

void processDiskOutput(const std::string &inputFile, const std::string &benchmarkName) {
    try {
         std::string modelName = getSystemModelName();
    
    std::cout << "System Model Name: " << modelName << std::endl;
    std::ifstream inFile(inputFile);
    std::string line;

    // Regular expression to split by one or more spaces
    std::regex regex("\\s+");
    std::vector<std::string> tokens;
    while (getline(inFile, line)) {
        if (line.find(benchmarkName) != std::string::npos) {
            
            std::istringstream iss(line);
            std::string word;

            // Process each word in the line
            while (iss >> word) {
                tokens.push_back(word);
            }

             if (tokens.size() >= 8) {
                    // Convert kB/s to MiB/s
                    // Extract the numeric part and convert it to float
                    size_t kPos6 = tokens[6].find("k");
                    if (kPos6 != std::string::npos) {
                        std::string numericPart = tokens[6].substr(0, kPos6);
                        float token6numericValue = std::stof(numericPart);

                        // Multiply by 1000
                        token6numericValue *= 1000;
                        tokens[6] = std::to_string((int)token6numericValue);
                        // Print the result
                        std::cout << "Converted value: " << token6numericValue << std::endl;
                    } else {
                        std::cerr << "Invalid format: no 'k' found in the string." << std::endl;
                    }

                    size_t kPos7 = tokens[7].find("k");
                    if (kPos7 != std::string::npos) {
                        std::string numericPart = tokens[7].substr(0, kPos7);
                        float token7numericValue = std::stof(numericPart);

                        // Multiply by 1000
                        token7numericValue *= 1000;
                        tokens[7] = std::to_string((int)token7numericValue);
                        // Print the result
                        std::cout << "Converted value: " << token7numericValue << std::endl;
                    } else {
                        std::cerr << "Invalid format: no 'k' found in the string." << std::endl;
                    }
                    
                   //  << tokens[2]  and  << tokens[2]  -- gives ms
                    std::cout << "Benchmark                                  : " << getFirstToken(tokens[0], '/') << std::endl;
                    std::cout << "Time  (ms)                                    : " << tokens[1] << std::endl;
                    std::cout << "CPU   (ms)                                     : " << tokens[3] << std::endl;
                    std::cout << "Iterations                                      : " << tokens[5] << std::endl;
                    std::cout << "Average Read (in MiB/s)             : " << tokens[6] << std::endl;
                    std::cout << "Average Write (in MiB/s)            : " << tokens[7] << std::endl;
                }
                tokens.clear(); // Clear tokens before reading the next line

        }
    }

    inFile.close();


    

       /* std::cout << "Preparing to insert into database..." << std::endl;
        // Establish a connection to the PostgreSQL "Disk" database
        pqxx::connection conn("dbname=benchmark_db user=postgres password=1234 hostaddr=10.16.39.156 port=5432");

        if (conn.is_open()) {
            std::cout << "Opened database successfully: " << conn.dbname() << std::endl;            

            // Specify the schema in the table creation and insertion queries
            // Create a SQL query with parameters
        std::string query1 = "INSERT INTO \"disk\".disk_table (sys_name, benchmark, time_ms, cpu_ms, iteration, averageread_mib_s, averagewrite_mib_s) VALUES ($1,$2, $3, $4, $5, $6, $7)";

        // Execute the query with the parameters
        pqxx::work txn(conn);
        txn.exec_params(query1,modelName, getFirstToken(tokens[0], '/'), tokens[1],std::stoi(tokens[3]),std::stoi(tokens[5]),std::stoi(tokens[6]),std::stoi(tokens[7]));
            //txn.exec("INSERT INTO \"dummy\".fio (id, name) VALUES (9,'fio');");
            txn.commit();
            std::cout << "insert to database successfully: " << conn.dbname() << std::endl;  
            conn.disconnect();
        } else {
            std::cerr << "Failed to open database." << std::endl;
        }*/
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

//Memory

void processMemoryOutput(const std::string &inputFile, const std::string &benchmarkName) {
    try {
        std::string modelName = getSystemModelName();
        std::cout << "System Model Name: " << modelName << std::endl;
        std::ifstream inFile(inputFile);
        std::string line;

        int avgIntMem = -1, avgFloatMem = -1, avgCache = -1;

        // Regular expression to split by one or more spaces
        std::regex regex("\\s+");
        
        while (getline(inFile, line)) {
            if (line.find(benchmarkName) != std::string::npos) {
                //std::cout << "Processing line: " << line << std::endl;
                std::istringstream iss(line);
                std::string word;
                std::vector<std::string> tokens;

                // Process each word in the line
                while (iss >> word) {
                    tokens.push_back(word);
                }

                if (!tokens.empty()) {
                    std::cout << "Benchmark                           : " << getFirstToken(tokens[0], '/') << std::endl;
                    std::cout << "Time_ms                              : " << tokens[1] << " " << tokens[2] << std::endl;
                    std::cout << "CPU_ms                               : " << tokens[3] << " " << tokens[4] << std::endl;
                    std::cout << "Iterations                           : " << tokens[5] << std::endl;
                    
                    if (benchmarkName.find("Benchmark_RAMSMP_INTmem") != std::string::npos) {
                        avgIntMem = std::stof(tokens[6]);
                        std::cout << "Average Integer Memory (in MiB/s)   : " << avgIntMem << std::endl;
                    } else if (benchmarkName.find("Benchmark_RAMSMP_FLOATmem") != std::string::npos) {
                        avgFloatMem = std::stof(tokens[6]);
                        std::cout << "Average Float Memory (in MiB/s)     : " << avgFloatMem << std::endl;
                    } else if (benchmarkName.find("BM_CacheBench") != std::string::npos) {
                        avgCache = std::stof(tokens[6]);
                        std::cout << "Average Cache (in MiB/s)            : " << avgCache << std::endl;
                    }
                }

                // DB Connection
               /* std::cout << "Preparing to insert into database..." << std::endl;
                pqxx::connection conn("dbname=benchmark_db user=postgres password=1234 hostaddr=10.16.39.156 port=5432");

                if (conn.is_open()) {
                    std::cout << "Opened database successfully: " << conn.dbname() << std::endl;

                    if (tokens.size() >= 6) {
                        std::string query = "INSERT INTO \"memory\".mem_table (sys_name, benchmark, time_ms, cpu_ms, iteration, average_int_mem_mibs, average_float_mem_mibs, average_cache_mibs) VALUES ($1, $2, $3, $4, $5, $6, $7, $8)";
                        pqxx::work txn(conn);   
                        
                        if (benchmarkName.find("Benchmark_RAMSMP_INTmem") != std::string::npos) {
                            txn.exec_params(query, modelName, getFirstToken(tokens[0], '/'), tokens[1], std::stoi(tokens[3]), std::stoi(tokens[5]), avgIntMem, -1, -1);
                        } else if (benchmarkName.find("Benchmark_RAMSMP_FLOATmem") != std::string::npos) {
                            txn.exec_params(query, modelName, getFirstToken(tokens[0], '/'), tokens[1], std::stoi(tokens[3]), std::stoi(tokens[5]), -1, avgFloatMem, -1);
                        } else if (benchmarkName.find("BM_CacheBench") != std::string::npos) {
                            txn.exec_params(query, modelName, getFirstToken(tokens[0], '/'), tokens[1], std::stoi(tokens[3]), std::stoi(tokens[5]), -1, -1, avgCache);
                        }

                        txn.commit();
                        std::cout << "Insert to database successful: " << conn.dbname() << std::endl;
                        conn.disconnect();
                    } 
                } else {
                    std::cerr << "Failed to open database." << std::endl;
                }*/
            }
        }

        inFile.close();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}


//network 

void processNetworkOutput(const std::string &inputFile, const std::string &benchmarkName) {
    try {
        std::string modelName = getSystemModelName();
        std::cout << "System Model Name: " << modelName << std::endl;
        std::ifstream inFile(inputFile);
        std::string line;

        int avgBand = -1, avgLat = -1, avgRec = -1,  avgSender = -1;

        // Regular expression to split by one or more spaces
        std::regex regex("\\s+");
        
        while (getline(inFile, line)) {
            if (line.find(benchmarkName) != std::string::npos) {
                //std::cout << "Processing line: " << line << std::endl;
                std::istringstream iss(line);
                std::string word;
                std::vector<std::string> tokens;

                // Process each word in the line
                while (iss >> word) {
                    tokens.push_back(word);
                }

                if (!tokens.empty()) {
                    std::cout << "Benchmark                           : " << getFirstToken(tokens[0], '/') << std::endl;
                    std::cout << "Time_ms                              : " << tokens[1] << " " << tokens[2] << std::endl;
                    std::cout << "CPU_ms                               : " << tokens[3] << " " << tokens[4] << std::endl;
                    std::cout << "Iterations                           : " << tokens[5] << std::endl;
                    
                    if (benchmarkName.find("BM_EthrBenchmark") != std::string::npos) {
                        avgBand = std::stof(tokens[6]);
                        std::cout << "Average Bandwidth (bits/sec)   : " << avgBand << std::endl;
                    } else if (benchmarkName.find("BM_SockperfLatencyUnderLoad") != std::string::npos) {
                        avgLat = std::stof(tokens[6]);
                        std::cout << "Average Latency (in usec)     : " << avgLat << std::endl;
                    } else if (benchmarkName.find("iperf") != std::string::npos) {
                        avgRec = std::stof(tokens[6]);
                        avgSender = std::stof(tokens[6]);
                        std::cout << "Average Receiver Bitrate (Mbits/sec)            : " << avgRec << std::endl;
                        std::cout << "Average Sender Bitrate (Mbits/sec)            : " << avgSender << std::endl;
                    }
                }

             // DB Connection for network
               /* std::cout << "Preparing to insert into database..." << std::endl;
                pqxx::connection conn("dbname=benchmark_db user=postgres password=1234 hostaddr=10.16.39.156 port=5432");

                if (conn.is_open()) {
                    std::cout << "Opened database successfully: " << conn.dbname() << std::endl;

                    if (tokens.size() >= 6) {
                        std::string query = "INSERT INTO \"network\".network_table (sys_name, benchmark, time_ms, cpu_ms, iteration, average_bandwidth_bits_s, average_latency_usec, average_sender_mbits, average_receiver_mbits) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)";
                        pqxx::work txn(conn);   
                        
                        if (benchmarkName.find("BM_EthrBenchmark") != std::string::npos) {
                            txn.exec_params(query, modelName, getFirstToken(tokens[0], '/'), tokens[1], std::stoi(tokens[3]), std::stoi(tokens[5]), avgBand, -1, -1, -1);
                        } else if (benchmarkName.find("BM_SockperfLatencyUnderLoad") != std::string::npos) {
                            txn.exec_params(query, modelName, getFirstToken(tokens[0], '/'), tokens[1], std::stoi(tokens[3]), std::stoi(tokens[5]), -1, avgLat, -1, -1);
                        } else if (benchmarkName.find("iperf") != std::string::npos) {
                            txn.exec_params(query, modelName, getFirstToken(tokens[0], '/'), tokens[1], std::stoi(tokens[3]), std::stoi(tokens[5]), -1, -1, avgRec, avgSender);
                        }

                        txn.commit();
                        std::cout << "Insert to database successful: " << conn.dbname() << std::endl;
                        conn.disconnect();
                    } 
                } else {
                    std::cerr << "Failed to open database." << std::endl;
                }*/
            }
        }

        inFile.close();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}


        






/*void saveBenchmarkResults(const std::string &inputFile, const std::string &outputFile, bool appendMode) {
    std::ios_base::openmode mode = appendMode ? std::ios::app : std::ios::out;
    std::ifstream inFile(inputFile);
    std::ofstream outFile(outputFile, mode);  // Open in specified mode
    std::string line;
    bool writeNextLine = false;

    while (getline(inFile, line)) {
        // Check for the line of dashes, indicating the next line is a header
        if (line.find("------") != std::string::npos) {
            writeNextLine = true;
            continue;
        }

        // Write the header line
        if (writeNextLine) {
            outFile << line << std::endl;  // Write header line
            writeNextLine = false;
            continue;
        }

        // Check if line contains benchmark result
        if (line.find("Benchmark_RAMSMP_INTmem") != std::string::npos ||
            line.find("Benchmark_RAMSMP_FLOATmem") != std::string::npos)
            // line.find("BM_CacheBench") != std::string::npos) 
            {
            outFile << line << std::endl;
        }
    }

    inFile.close();
    outFile.close();
}*/

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <option>\n";
        return 1;
    }

    std::string option = argv[1];

        if (option == "disk") {
            system("./fio-sync-test.sh | tee fio-sync-test_output.txt > /dev/null 2>&1");    
            system("./test1.sh | tee test1_output.txt > /dev/null 2>&1");
            
            std::cout << "\n\n";
            std::cout << "######## BENCHMARK RESULTS ########\n";
            std::cout << "1. BM_WrapFio \n";
            processDiskOutput("fio-sync-test_output.txt", "BM_WrapFio/iterations:10");
            std::cout << "\n";
            std::cout << "2. FIO\n";
            processDiskOutput("test1_output.txt", "fio/iterations:10");
            std::cout << "######## END ########\n";

         } else if (option == "memory") {
            system("./ramsmp.sh | tee ramsmp_output.txt > /dev/null 2>&1");
            system("./cachebench.sh | tee cache_output.txt > /dev/null 2>&1");

            std::cout << "\n\n";
            std::cout << "######## BENCHMARK RESULTS ########\n";
            std::cout << "1. RAMSMP_INT_MEM\n";
            processMemoryOutput("ramsmp_output.txt", "Benchmark_RAMSMP_INTmem/iterations:10");
            std::cout << "\n";
            std::cout << "2. RAMSMP_FLOAT_MEM\n";
            processMemoryOutput("ramsmp_output.txt", "Benchmark_RAMSMP_FLOATmem/iterations:10");
            std::cout << "\n";
            std::cout << "3. CACHEBENCH\n";
            processMemoryOutput("cache_output.txt", "BM_CacheBench/iterations:3");
            
           
         }

        else if (option == "network") {

            system("./ethr-test.sh | tee ethrr_output.txt > /dev/null 2>&1");
            system("./sock-test.sh | tee sockk_output.txt > /dev/null 2>&1");
            system("./iperf-test.sh | tee iperff_output.txt > /dev/null 2>&1");

            std::cout << "\n\n";
            std::cout << "######## BENCHMARK RESULTS ########\n";
            std::cout << "1. ETHR\n";
            processNetworkOutput("ethrr_output.txt", "BM_EthrBenchmark/iterations:2");
            std::cout << "\n";
            std::cout << "2. SOCKPERF\n";
            processNetworkOutput("sockk_output.txt", "BM_SockperfLatencyUnderLoad/iterations:2");
            std::cout << "\n";
            std::cout << "3. IPERF\n";
            processNetworkOutput("iperff_output.txt", "iperf/iterations:3");
            

            
        
        /*case 4:
            system("./fio-sync-test.sh | tee fio_output.txt");
            system("./test1.sh | tee test1_output.txt");
            //saveBenchmarkResults("fio_output.txt", "disk_results.txt", false); // Overwrite mode for the first file
            //saveBenchmarkResults("test1_output.txt", "disk_results.txt", true); // Append mode for the second file

            system("./ramsmp.sh | tee ramsmp_output.txt");
            system("./cachebench.sh | tee cachebench_output.txt");
            //saveBenchmarkResults("ramsmp_output.txt", "memory_results.txt", false); // Overwrite mode for the first file
            //saveBenchmarkResults("cachebench_output.txt", "memory_results.txt", true); // Append mode for the second file

            break;
            
            
        default:
            std::cout << "Invalid choice!\n";
            break;*/
    }



    return 0;
}
