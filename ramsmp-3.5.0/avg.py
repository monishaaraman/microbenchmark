def extract_and_average(input_filename):
    total = 0
    count = 0

    # Open the input file and read line by line
    with open(input_filename, 'r') as file:
        for line in file:
            # Check if the line contains the FL-POINT AVERAGE
            if "FL-POINT  AVERAGE:" in line:
                # Extract the average value and convert it to float
                try:
                    average_value = float(line.split()[-2])  # Assumes the value is the second last word
                    #print(f"Found average: {average_value}")  # Debug print
                    total += average_value
                    count += 1
                except ValueError as e:
                    # In case of a conversion error, print the error message
                    print(f"Could not convert to float: {line}")
                    print(e)

    # After reading all lines, check if any averages were found
    if count == 0:
        raise ValueError("No 'FL-POINT AVERAGE' lines found in the file.")

    # Calculate the overall average
    overall_average = total / count
    print(f"Overall average of FLOATMEM (MB/s): {overall_average:.3f}")  # Debug print
    return overall_average

def write_result_to_file(output_filename, result):
    # Open the output file and write the overall average
    with open(output_filename, 'w') as file:
        file.write(f"Overall FL-POINT AVERAGE: {result}\n")
        #print(f"Result written to {output_filename}")  # Debug print

# Main execution
if __name__ == "__main__":
    input_filename = 'out.txt'
    output_filename = 'res.txt'
    
    try:
        # Extract the averages and calculate the overall average
        overall_avg = extract_and_average(input_filename)

        # Write the overall average to the output file
        write_result_to_file(output_filename, overall_avg)
    except Exception as e:
        print(f"An error occurred: {e}")
