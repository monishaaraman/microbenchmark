from statistics import mean
import os

bw_read, bw_write = [], []

with open("run_status_output.txt", "r") as file:
    for line in file:
        if "READ" in line:
            words = line.split()
            bw_value = (words[1].replace("bw=", ""))[:-5]
            bw_read.append(int(float(bw_value)))
        elif "WRITE" in line:
            words = line.split()
            bw_value = (words[1].replace("bw=", ""))[:-5]
            bw_write.append(int(float(bw_value)))

#print("Average Read value result:", int(mean(bw_read)))
#print("Average Write value result:", int(mean(bw_write)))

# Set environment variables within the Python script
os.environ["Fio_Average_Read"] = str(int(mean(bw_read)))
os.environ["Fio_Average_Write"] = str(int(mean(bw_write)))

# If you still want to write the values to the shell script, you can do so
with open("set_env_fio.sh", "w") as file:
    file.write(f"#!/bin/bash\nexport Fio_Average_Read={str(int(mean(bw_read)))}\n")
    file.write(f"export Fio_Average_Write={str(int(mean(bw_write)))}\n")

os.system("chmod +x set_env_fio.sh")
#print(f"{int(mean(bw_read))} {int(mean(bw_write))}")

with open("average_values.txt", "w") as file:
    file.write(f"{int(mean(bw_read))}\n")
    file.write(f"{int(mean(bw_write))}\n")
