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
DIR="$(pwd)/ethr-bench"

# Check if the ethr-bench directory exists
if [ -d "$DIR" ]; then
    cd "$DIR"
    
    # Check if ethr is already built
    if [ ! -f "ethr" ]; then
        echo "ethr bin is missing!"
    else
        echo "fio is already built in $DIR"
    fi
else
    echo "Directory $DIR does not exist."
    exit 1
fi

# Create the required files and set the appropriate permissions
FILES=("ethres.txt" "ethroutput.txt" "ethr-average.py" )

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
        
    # Check if ethrbench is present and executable
    if [ -f "./ethrbench" ] && [ -x "./ethrbench" ]; then
        echo "Running ethrbench..."
	sudo chown $USER:$USER ethrbench
        sudo ./ethrbench --benchmark_counters_tabular=true
    else
        echo "ethrbench is not found or not executable in $DIR"
        exit 1
    fi
else
    echo "Directory $DIR does not exist."
    exit 1
fi

