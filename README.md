# 🛠️ Page Fault Detection and Optimization System

This project is a Linux-based solution designed to detect and monitor page fault behavior in real-time, providing insights into system memory access patterns. It simulates and mitigates minor page faults and visualizes the performance before and after optimization using graphs. Built using **C++**, **Python**, **Flask**, and **Matplotlib**, the system is ideal for performance analysis and debugging in low-level operating environments.
project-root/![WhatsApp Image 2025-05-27 at 1 41 06 AM](https://github.com/user-attachments/assets/ae35b80e-5987-46e7-8663-7fdf9afb46d3)
![WhatsApp Image 2025-05-27 at 1 40 09 AM](https://github.com/user-attachments/assets/6e3218f1-f219-4618-8a59-9e53a01d9701)

## 📌 Features

- 📉 **Real-time Page Fault Logging** (Minor/Major)
- 📊 **Graphical Visualization** using Matplotlib
- 🧠 **Memory Stats Monitoring**: VmSize, VmRSS, VmSwap
- ⚙️ **Fault Simulation and Optimization** to reduce minor faults
- 🖥️ **Web Interface** using Flask for running and viewing results

## 🧰 Tech Stack

- **Languages:** C++, Python 3
- **Frameworks:** Flask (Python)
- **Tools:** Matplotlib, /proc filesystem, Shell & Linux system calls
- **OS Environment:** Ubuntu/Linux

## 🚀 Getting Started

### 🔧 Prerequisites

- Linux OS (preferably Ubuntu)
- Python 3 installed
- g++ compiler
- Flask and Matplotlib installed (`pip install flask matplotlib`)

### 📂 Folder Structure
├── page_fault_detection.cpp
├── page_fault_solution.cpp
├── plot_page_fault_detection.py
├── plot_page_fault_sol.py
├── page_fault_test_log.txt
├── page_fault_solution_log.txt
├── app.py
└── templates/
└── index.html
