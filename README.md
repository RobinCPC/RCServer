# RCServer
ROS interface for NexCOBOT General Robot Controller (GRC)

## Development Environment
* Windows 7 32-bit or Windows 10 64-bit
* Visual Studio C++ 2015/17
* CMake 3.2 or later

## Build Instruction

### Build with CMake GUI
1. Clone this repo.
2. Create a `build` folder.
3. Use CMake gui to generate Visual Studio Solution file in `build` folder.
4. Open solution file with Visual Studio and build `main` project.
5. Copy `config.json` to `C:\NEXCOBOT` directory.
6. run `main.exe`

### Build with Command Prompt for VS2015/2017
1. Clone this repo.
	`git clone https://github.com/RobinCPC/RCServer.git`
2. Create a `build` fodler in this repo directory
	`cd RCServer && mkdir build && cd build`
3. Use CMake CLI to generate Visual Studio Solution in `build` folder. (For VS2015, x64)
	`cmake -G "Visual Studio 14 2015" -A x64 -S .. -B "build64"`
4. Build the solution with CMAKE CLI (For Release)
	`cmake --build build64 --config Release`
5. Copy `config.json` to `C:\NEXCOBOT` directory.
6. run `main.exe`. The file is located in `build\build64\Release` directory.
	`.\buld64\Release\main.exe`

## Dependency (third-party)
* [JSON for Modern C++](https://github.com/nlohmann/json)

