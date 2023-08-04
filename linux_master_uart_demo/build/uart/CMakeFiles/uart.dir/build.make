# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/user/workspace/r_boot_demos/uart_demo

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/user/workspace/r_boot_demos/uart_demo/build

# Include any dependencies generated for this target.
include uart/CMakeFiles/uart.dir/depend.make

# Include the progress variables for this target.
include uart/CMakeFiles/uart.dir/progress.make

# Include the compile flags for this target's objects.
include uart/CMakeFiles/uart.dir/flags.make

uart/CMakeFiles/uart.dir/uart.c.o: uart/CMakeFiles/uart.dir/flags.make
uart/CMakeFiles/uart.dir/uart.c.o: ../uart/uart.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/user/workspace/r_boot_demos/uart_demo/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object uart/CMakeFiles/uart.dir/uart.c.o"
	cd /home/user/workspace/r_boot_demos/uart_demo/build/uart && /home/user/tools/esm6800/sysroots/x86_64-pokysdk-linux/usr/bin/arm-emtronix-linux-gnueabi/arm-emtronix-linux-gnueabi-gcc   -march=armv7ve -mfpu=neon  -mfloat-abi=hard -mcpu=cortex-a7 --sysroot=/home/user/tools/esm6800/sysroots/cortexa7hf-neon-emtronix-linux-gnueabi $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/uart.dir/uart.c.o   -c /home/user/workspace/r_boot_demos/uart_demo/uart/uart.c

uart/CMakeFiles/uart.dir/uart.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/uart.dir/uart.c.i"
	cd /home/user/workspace/r_boot_demos/uart_demo/build/uart && /home/user/tools/esm6800/sysroots/x86_64-pokysdk-linux/usr/bin/arm-emtronix-linux-gnueabi/arm-emtronix-linux-gnueabi-gcc   -march=armv7ve -mfpu=neon  -mfloat-abi=hard -mcpu=cortex-a7 --sysroot=/home/user/tools/esm6800/sysroots/cortexa7hf-neon-emtronix-linux-gnueabi $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/user/workspace/r_boot_demos/uart_demo/uart/uart.c > CMakeFiles/uart.dir/uart.c.i

uart/CMakeFiles/uart.dir/uart.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/uart.dir/uart.c.s"
	cd /home/user/workspace/r_boot_demos/uart_demo/build/uart && /home/user/tools/esm6800/sysroots/x86_64-pokysdk-linux/usr/bin/arm-emtronix-linux-gnueabi/arm-emtronix-linux-gnueabi-gcc   -march=armv7ve -mfpu=neon  -mfloat-abi=hard -mcpu=cortex-a7 --sysroot=/home/user/tools/esm6800/sysroots/cortexa7hf-neon-emtronix-linux-gnueabi $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/user/workspace/r_boot_demos/uart_demo/uart/uart.c -o CMakeFiles/uart.dir/uart.c.s

uart/CMakeFiles/uart.dir/uart.c.o.requires:

.PHONY : uart/CMakeFiles/uart.dir/uart.c.o.requires

uart/CMakeFiles/uart.dir/uart.c.o.provides: uart/CMakeFiles/uart.dir/uart.c.o.requires
	$(MAKE) -f uart/CMakeFiles/uart.dir/build.make uart/CMakeFiles/uart.dir/uart.c.o.provides.build
.PHONY : uart/CMakeFiles/uart.dir/uart.c.o.provides

uart/CMakeFiles/uart.dir/uart.c.o.provides.build: uart/CMakeFiles/uart.dir/uart.c.o


# Object files for target uart
uart_OBJECTS = \
"CMakeFiles/uart.dir/uart.c.o"

# External object files for target uart
uart_EXTERNAL_OBJECTS =

uart/libuart.a: uart/CMakeFiles/uart.dir/uart.c.o
uart/libuart.a: uart/CMakeFiles/uart.dir/build.make
uart/libuart.a: uart/CMakeFiles/uart.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/user/workspace/r_boot_demos/uart_demo/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libuart.a"
	cd /home/user/workspace/r_boot_demos/uart_demo/build/uart && $(CMAKE_COMMAND) -P CMakeFiles/uart.dir/cmake_clean_target.cmake
	cd /home/user/workspace/r_boot_demos/uart_demo/build/uart && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/uart.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
uart/CMakeFiles/uart.dir/build: uart/libuart.a

.PHONY : uart/CMakeFiles/uart.dir/build

uart/CMakeFiles/uart.dir/requires: uart/CMakeFiles/uart.dir/uart.c.o.requires

.PHONY : uart/CMakeFiles/uart.dir/requires

uart/CMakeFiles/uart.dir/clean:
	cd /home/user/workspace/r_boot_demos/uart_demo/build/uart && $(CMAKE_COMMAND) -P CMakeFiles/uart.dir/cmake_clean.cmake
.PHONY : uart/CMakeFiles/uart.dir/clean

uart/CMakeFiles/uart.dir/depend:
	cd /home/user/workspace/r_boot_demos/uart_demo/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/user/workspace/r_boot_demos/uart_demo /home/user/workspace/r_boot_demos/uart_demo/uart /home/user/workspace/r_boot_demos/uart_demo/build /home/user/workspace/r_boot_demos/uart_demo/build/uart /home/user/workspace/r_boot_demos/uart_demo/build/uart/CMakeFiles/uart.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : uart/CMakeFiles/uart.dir/depend
