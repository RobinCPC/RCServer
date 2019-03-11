project(RCServer)
cmake_minimum_required(VERSION 3.5)
add_definitions(-std=c++11)

# Set to Build Release
set(CMAKE_CONFIGURATION_TYPES "Release;Debug")

# Check System Environment
cmake_host_system_information(RESULT ENV QUERY IS_64BIT)

# Choose 32bit or 64bit
IF (ENV)
message(STATUS  "Windows 64-bit environment")
SET(NEXMOTION_INCLUDE_DIR "C:\\Program Files (x86)\\NEXCOBOT\\NexGMC\\Header")
SET(NEXMOTION_LIBS  "C:\\Program Files (x86)\\NEXCOBOT\\NexGMC\\Lib" )

ELSE()
message(STATUS  "Windows 32-bit environment")
SET(NEXMOTION_INCLUDE_DIR "C:\\Program Files\\NEXCOBOT\\NexGMC\\Header")
SET(NEXMOTION_LIBS  "C:\\Program Files\\NEXCOBOT\\NexGMC\\Lib" )

ENDIF()

# include head file
include_directories(
    #    include
    ${NEXMOTION_INCLUDE_DIR}
    )


add_library(RCServer
    src/rc_server.cpp src/rc_server.h
)

add_library(Motion
	src/motion.cpp src/motion.h
)

add_executable(main
    src/main.cpp src/rc_interface.cpp src/rc_interface.h
)
# Link executable file with NexMotion.lib
target_link_libraries(main RCServer Motion ${NEXMOTION_LIBS}/NexMotion.lib)
