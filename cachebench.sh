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

# Define the path to the cachebench binary
CACHEBENCH_BIN="$PWD/llcbench/cachebench/cachebench"

# Check if cachebench binary exists
if [ -f "$CACHEBENCH_BIN" ]; then
    echo "Cachebench binary already exists."
else
   
    echo "Cachebench binary not found. Starting build process."

    sed -i '.orig' '/#include <malloc.h>/d' cachebench.c

    # Navigate to the llcbench directory
    cd "$PWD/llcbench" || exit

    # Execute build commands
    sudo make linux-mpich
    sudo make cache-bench

    # Check if the build was successful
    if [ -f "$CACHEBENCH_BIN" ]; then
        echo "Cachebench build completed successfully."
    else
        echo "Cachebench build failed."
        exit 1
    fi
fi
# Create the required files and set the appropriate permissions
FILES=("cacheoutput.txt" "cacheresults.txt" "average-cache.py")

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
# Build Google Benchmark in the fio-3.35 directory
DIR="$PWD/cachebench"
if [ -d "$DIR" ]; then
    cd "$DIR"
 # Build Google Benchmark
    echo "Building Benchmark..."
    sudo cmake .
    echo "cmake done" 
    sudo make
    echo "make done"

# Check if cacheebench is present and executable
    if [ -f "./cacheebench" ] && [ -x "./cacheebench" ]; then
        echo "Running cacheebench..."
	sudo chown $USER:$(id -gn $USER) "cacheebench"
       sudo ./cacheebench --benchmark_counters_tabular=true
    else
        echo "cacheebench is not found or not executable in $DIR"
        exit 1
    fi
else
    echo "Directory $DIR does not exist."
    exit 1
fi
