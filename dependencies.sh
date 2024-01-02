#!/bin/bash

# Update package list
sudo apt-get update

# Install dependencies if not already installed
DEPS=("cmake" "libbenchmark-dev" "python3" "git" "libaio-dev" "autoconf" "libpqxx-dev")

for dep in "${DEPS[@]}"; do
    if ! dpkg -l | grep -q "^ii  $dep"; then
        echo "Installing $dep..."
        sudo apt-get install -y "$dep"
        sleep 5 # Introduce a 5-second delay (adjust as needed)
    else
        echo "$dep is already installed."
    fi
done

