#!/bin/bash

# Install dependencies if not already installed
DEPS=("cmake" "libbenchmark-dev" "python3" "git" "libaio-dev")

for dep in "${DEPS[@]}"; do
    if ! dpkg -l | grep -q "^ii  $dep"; then
        echo "Installing $dep..."
        sudo apt-get install -y "$dep"
    else
        echo "$dep is already installed."
    fi
done

#!/bin/bash

# Define the path to the iperf-3.14 binary
IPERF_BIN="$PWD/iperf-3.14/src/iperf3"

# Check if iperf binary exists
if [ -f "$IPERF_BIN" ]; then
    echo "iperf binary already exists."
else
    echo "iperf binary not found. Starting build process."

    # Navigate to the iperf-3.14 directory
    cd "$PWD/iperf-3.14" || exit

    # Execute build commands
    CFLAGS="$CFLAGS -O3 -march=native"
    echo "cflag done"
    sudo ./configure --prefix=$HOME/iperf-install CFLAGS="$CFLAGS" 
    echo "configure done"
    sudo make -j $NUM_CPU_CORES
    echo "make done"
    sudo make install
    echo "make install done"

    # Check if the build was successful
    if [ -f "$IPERF_BIN" ]; then
        echo "iperf build completed successfully."
    else
        echo "iperf build failed."
        exit 1
    fi
fi

# Create the required files and set the appropriate permissions
FILES=("iperf_results.txt")

for file in "${FILES[@]}"; do
    FILE_PATH="$DIR/$file"
    echo "Checking file $FILE_PATH..."  # Added for debugging
    if [ ! -f "$FILE_PATH" ]; then
        echo "Creating $file..."
        sudo touch "$FILE_PATH"
        sudo chown $USER:$(id -gn $USER) "$FILE_PATH"
    else
    	sudo chown $USER:$(id -gn $USER) "$FILE_PATH"
        echo "$file already exists."
        
    fi
done

# Build Google Benchmark in the iperf-3.14/src directory
DIR="$PWD/src"
if [ -d "$DIR" ]; then
    cd "$DIR"
 # Build Google Benchmark
    echo "Building Benchmark..."
    sudo cmake .
    echo "cmake done" 
    sudo make
    echo "make done"

# Check if iperfbench is present and executable
    if [ -f "./iperfbench" ] && [ -x "./iperfbench" ]; then
        echo "Running iperfbench..."
	sudo chown $USER:$(id -gn $USER) "iperfbench"
       sudo ./iperfbench --benchmark_counters_tabular=true
    else
        echo "iperfbench is not found or not executable in $DIR"
        exit 1
    fi
else
    echo "Directory $DIR does not exist."
    exit 1
fi
