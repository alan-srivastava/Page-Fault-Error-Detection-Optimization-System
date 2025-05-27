import matplotlib.pyplot as plt
from datetime import datetime


timestamps = []
minor_faults = []
major_faults = []
vm_size = []
vm_rss = []
vm_swap = []


with open("page_fault_test_log.txt", "r") as file:
    lines = file.readlines()

i = 0
while i < len(lines):
    if lines[i].startswith("["):
       
        parts = lines[i].strip().split("]")
        time_str = parts[0][1:]
        rest = parts[1]

        timestamps.append(datetime.strptime(time_str, "%Y-%m-%d %H:%M:%S"))
        minor_faults.append(int(rest.split("Minor Faults:")[1].split("|")[0].strip()))
        major_faults.append(int(rest.split("Major Faults:")[1].split("|")[0].strip()))

        
        vm_size_val = int(lines[i+1].split()[1])
        vm_rss_val = int(lines[i+2].split()[1])
        vm_swap_val = int(lines[i+3].split()[1])

        vm_size.append(vm_size_val)
        vm_rss.append(vm_rss_val)
        vm_swap.append(vm_swap_val)

        i += 5  
    else:
        i += 1

# Plotting
plt.figure(figsize=(12, 6))

plt.subplot(2, 1, 1)
plt.plot(timestamps, minor_faults, label="Minor Faults", marker='o')
plt.plot(timestamps, major_faults, label="Major Faults", marker='x')
plt.ylabel("Page Faults")
plt.legend()
plt.grid(True)

plt.subplot(2, 1, 2)
plt.plot(timestamps, vm_size, label="VmSize (kB)", linestyle="--")
plt.plot(timestamps, vm_rss, label="VmRSS (kB)", linestyle="-")
plt.plot(timestamps, vm_swap, label="VmSwap (kB)", linestyle=":")
plt.ylabel("Memory (kB)")
plt.xlabel("Time")
plt.legend()
plt.grid(True)

plt.tight_layout()
plt.show()