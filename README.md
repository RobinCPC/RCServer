# RCServer
ROS interface for NexCOBOT General Robot Controller (GRC)

## Development Environment
* Windows 7 32-bit or Windows 10 64-bit
* Visual Studio C++ 2015/17
* CMake 3.2 or later

## Build Instruction
1. Clone this repo.
2. Create a `build` folder.
3. Use CMake gui to generate Visual Studio Solution file in `build` folder.
4. Open solution file with Visual Studio and build `main` project.
5. Copy `config.json` to `C:\NEXCOBOT` directory.
6. run `main.exe`

## Dependency (third-party)
* [JSON for Modern C++](https://github.com/nlohmann/json)

