import os

def extract_average_latency(input_filename, output_filename):
    total_latency = 0.0
    count = 0

    with open(input_filename, 'r') as file:
        for line in file:
            if '====> avg-latency=' in line:
                start = line.find('====> avg-latency=') + len('====> avg-latency=')
                end = line.find(' ', start)
                latency_str = line[start:end]
                try:
                    latency = float(latency_str)
                    total_latency += latency
                    count += 1
                except ValueError:
                    print(f"Could not convert '{latency_str}' to float")

    if count > 0:
        overall_average = total_latency / count
        
        # Use os.path.join to create dynamic file paths
        result_path = os.path.join(current_dir, output_filename)
        
        with open(result_path, 'w') as res_file:
            res_file.write(f"Overall Average Latency: {overall_average} usec\n")
    else:
        with open(output_filename, 'w') as res_file:
            res_file.write("No average latency data found")

# Current working directory
current_dir = os.getcwd()

# Define input and output filenames separately
input_filename = os.path.join(current_dir, "sockout.txt")
output_filename = os.path.join(current_dir, "sockres.txt")

# Call the function with dynamic file paths
extract_average_latency(input_filename, output_filename)
