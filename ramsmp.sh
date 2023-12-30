#!/bin/bash

# Install dependencies if not already installed
DEPS=("cmake" "python3" "git" )

for dep in "${DEPS[@]}"; do
    if ! dpkg -l | grep -q "^ii  $dep"; then
        echo "Installing $dep..."
        sudo apt-get install -y "$dep"
    else
        echo "$dep is already installed."
    fi
done

# Define the directory
DIR="$(pwd)/ramsmp-3.5.0"

# Check if the ramsmp directory exists
if [ -d "$DIR" ]; then
    cd "$DIR"
    
    # Check if fio is already built
    if [ ! -f "ramsmp" ]; then
        echo "Building ramsmp..."
        export CFLAGS="-O3 -march=native $CFLAGS"
        echo "building...ramsmp!!"

        sudo cc $CFLAGS -o ramsmp fltmark.c fltmem.c intmark.c intmem.c ramsmp.c

	echo "./build done" 
    else
        echo "ramsmp is already built in $DIR"
    fi
else
    echo "Directory $DIR does not exist."
    exit 1
fi


# Create the required files and set the appropriate permissions
FILES=("result.txt" "res.txt" "output.txt" "out.txt" "average.py" "avg.py"  )

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

# Define the build directory for Google Benchmark
BENCHMARK_DIR="$(pwd)"



# Build Google Benchmark in the ramsmp-3.5.0 directory
if [ -d "$DIR" ]; then
    cd "$DIR"
    
    # Build Google Benchmark
    echo "Building Benchmark..."
    sudo cmake . && sudo make
    
    # Check if ramsmp is present and executable
    if [ -f "./bench" ] && [ -x "./bench" ]; then
        echo "Running ramsmp..."
	sudo chown $USER:$(id -gn $USER) "bench"
        ./bench --benchmark_counters_tabular=true
    else
        echo "bench is not found or not executable in $DIR"
        exit 1
    fi
else
    echo "Directory $DIR does not exist."
    exit 1
fi
