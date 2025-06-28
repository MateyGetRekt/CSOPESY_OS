# CSOPESY_OS

**Process Multiplexer and CLI**

Milan, John Lloyd - S22

Quiambao, Mark Kobe - S22

Medina, Lance Miguel - S22

Villanueva, Ryan - S15

## Instructions

1. **Place the `config.txt`**  
   Make sure your `config.txt` in the same folder as your source/executable.

2. **In Visual Studio Code**  
   - Open the project folder (the one containing `CSOPESY_MCO1.cpp`).  
   - Press **Ctrl + Shift + B** to build (runs your default “Build CSOPESY” task).  
   - Press **F5** to launch and run (or debug) the emulator.

3. **Via Terminal (fallback)**  
   ```bash
   g++ -g -std=c++17 CSOPESY_MCO1.cpp -o csopesy -pthread
   ./csopesy
