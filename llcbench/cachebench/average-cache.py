def extract_mb_sec_values(filename):
    mb_sec_values = []

    with open(filename, 'r') as file:
        for line in file:
            parts = line.split()
            if len(parts) >= 2:
                try:
                    mb_sec = float(parts[1])  # MB/sec is the second element
                    mb_sec_values.append(mb_sec)
                except ValueError:
                    pass  # Skip lines where conversion to float might fail

    return mb_sec_values

def compute_average(values):
    if values:
        return sum(values) / len(values)
    else:
        return None

def write_to_file(content, output_filename):
    with open(output_filename, 'w') as file:
        file.write(content)

if __name__ == "__main__":
    mb_sec_values = extract_mb_sec_values("cacheoutput.txt")
    average_mb_sec = compute_average(mb_sec_values)

    if average_mb_sec is not None:
        result = f"Overall average (MB/s): {average_mb_sec:.3f}"
    else:
        result = "No MB/sec data found."

    write_to_file(result, "cacheresults.txt")
    print(result)
