import json
import os

# Current working directory
current_dir = os.getcwd()

# Construct the file paths
filename = os.path.join(current_dir, 'ethroutput.txt')
result_filename = os.path.join(current_dir, 'ethres.txt')


def extract_bandwidth(line):
    try:
        data = json.loads(line)
        if 'Protocol' in data and data['Protocol'] == 'TCP' and 'BitsPerSecond' in data:
            value_str = data['BitsPerSecond']
            if value_str.endswith('G'):
                return float(value_str[:-1]) * 1e9  # Assuming G stands for Gigabits
            return float(value_str)
    except json.JSONDecodeError:
        return None

def calculate_average_bandwidth(filename):
    bandwidth_values = []
    with open(filename, 'r') as file:
        for line in file:
            value = extract_bandwidth(line)
            if value is not None:
                bandwidth_values.append(value)

    if bandwidth_values:
        average = sum(bandwidth_values) / len(bandwidth_values)
        return round(average, 2)  # Rounding to two decimal places
    else:
        return None

# Current working directory
current_dir = os.getcwd()

# Construct the file paths
filename = os.path.join(current_dir, 'ethroutput.txt')
average_bandwidth = calculate_average_bandwidth(filename)

# Construct the result file path
result_filename = os.path.join(current_dir, 'ethres.txt')

# Writing the average bandwidth to ethres.txt
if average_bandwidth is not None:
    print(f"Average Bandwidth: {average_bandwidth} bits/sec")
    with open(result_filename, 'w') as result_file:
        result_file.write(f"Average Bandwidth: {average_bandwidth} bits/sec")
else:
    print("No bandwidth data found.")


