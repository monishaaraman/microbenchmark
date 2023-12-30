def extract_averages(filename):
    averages = []

    with open(filename, 'r') as f:
        for line in f:
            if "INTEGER   AVERAGE:" in line:
                # Split the line by spaces and get the third item (index 2)
                avg_value = float(line.split()[2])
                averages.append(avg_value)

    return averages

def compute_average(filename):
    averages = extract_averages(filename)

    if not averages:
        return "No average values found!"

    total_avg = sum(averages) / len(averages)
    return f"Overall average of INTMEM (MB/s): {total_avg:.3f}"

def write_to_file(content, output_filename):
    with open(output_filename, 'w') as f:
        f.write(content)

if __name__ == "__main__":
    result = compute_average("output.txt")
    write_to_file(result, "result.txt")
    print(result)
