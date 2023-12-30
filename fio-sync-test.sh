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
DIR="$(pwd)/fio-sync"

# Check if the fio-3.35 directory exists
if [ -d "$DIR" ]; then
    cd "$DIR"
    
    # Check if fio is already built
    if [ ! -f "fio" ]; then
        echo "Building fio..."
        sudo ./configure --extra-cflags="-O3 -fcommon"
	echo "./configure - run complete"
        sudo make 
	echo "make - complete"
        sudo make install
	echo "make install - complete"
    else
        echo "fio is already built in $DIR"
    fi
else
    echo "Directory $DIR does not exist."
    exit 1
fi


# Create the required files and set the appropriate permissions
FILES=("run_status_output.txt" "average_values.txt" "PhoronixFio1.py" "set_env_fio.sh"  )

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
    
    echo -e "\e[1;36m****The setup for this fio test involves performing RANDOM READ & WRITE operations using the SYNCHRONOUS engine, utilizing an 8MB block size, and with DIRECT I/O enabled, without using a buffer.****\e[0m"
    
    # Check if WrapFioBenchmark is present and executable
    if [ -f "./WrapFioBenchmark" ] && [ -x "./WrapFioBenchmark" ]; then
        echo "Running WrapFioBenchmark..."
	sudo chown $USER:$USER WrapFioBenchmark
        sudo ./WrapFioBenchmark --benchmark_counters_tabular=true 
    else
        echo "WrapFioBenchmark is not found or not executable in $DIR"
        exit 1
    fi
else
    echo "Directory $DIR does not exist."
    exit 1
fi
