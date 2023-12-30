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

# Define the directory
DIR="$(pwd)/sockperf-3.7"

# Check if the sockperf-3.7 directory exists
if [ -d "$DIR" ]; then
    cd "$DIR"
    
    # Check if sockperf is already built
    if [ ! -f "sockperf" ]; then
        echo "Building sockperf..."
        sudo ./autogen.sh
	echo "./autogen - run complete"
        sudo ./configure 
	echo "./configure - complete"
        sudo make 
	echo "make - complete"
    else
        echo "sockperf is already built in $DIR"
    fi
else
    echo "Directory $DIR does not exist."
    exit 1
fi

# Create the required files and set the appropriate permissions
FILES=("sockout.txt" "sockres.txt" "sock-avg.py"  )

for file in "${FILES[@]}"; do
    FILE_PATH="$DIR/$file"
    echo "Checking file $FILE_PATH..."  # Added for debugging
    if [ ! -f "$FILE_PATH" ]; then
        echo "Creating $file..."
        sudo touch "$FILE_PATH"
        sudo chown $USER:$USER "$FILE_PATH"
    else
    	sudo chown $USER:$USER "$FILE_PATH"
        echo "$file already exists."
        
    fi
done

# Build Google Benchmark in the fio-3.35 directory
if [ -d "$DIR" ]; then
    cd "$DIR"
    
    # Build Google Benchmark
    echo "Building Benchmark..."
    sudo cmake . && sudo make
    
    
    # Check if sockperfbench is present and executable
    if [ -f "./sockperfbench" ] && [ -x "./sockperfbench" ]; then
        echo "Running sockperfbench..."
	sudo chown $USER:$USER sockperfbench
        sudo ./sockperfbench --benchmark_counters_tabular=true
    else
        echo "sockperfbench is not found or not executable in $DIR"
        exit 1
    fi
else
    echo "Directory $DIR does not exist."
    exit 1
fi


