# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.23

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/d/C/GW

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/d/C/GW/cmake-build-release

# Include any dependencies generated for this target.
include CMakeFiles/GW.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/GW.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/GW.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/GW.dir/flags.make

CMakeFiles/GW.dir/src/main.c.o: CMakeFiles/GW.dir/flags.make
CMakeFiles/GW.dir/src/main.c.o: ../src/main.c
CMakeFiles/GW.dir/src/main.c.o: CMakeFiles/GW.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/C/GW/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/GW.dir/src/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/GW.dir/src/main.c.o -MF CMakeFiles/GW.dir/src/main.c.o.d -o CMakeFiles/GW.dir/src/main.c.o -c /mnt/d/C/GW/src/main.c

CMakeFiles/GW.dir/src/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/GW.dir/src/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/C/GW/src/main.c > CMakeFiles/GW.dir/src/main.c.i

CMakeFiles/GW.dir/src/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/GW.dir/src/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/C/GW/src/main.c -o CMakeFiles/GW.dir/src/main.c.s

# Object files for target GW
GW_OBJECTS = \
"CMakeFiles/GW.dir/src/main.c.o"

# External object files for target GW
GW_EXTERNAL_OBJECTS =

GW: CMakeFiles/GW.dir/src/main.c.o
GW: CMakeFiles/GW.dir/build.make
GW: CMakeFiles/GW.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/d/C/GW/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable GW"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/GW.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/GW.dir/build: GW
.PHONY : CMakeFiles/GW.dir/build

CMakeFiles/GW.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/GW.dir/cmake_clean.cmake
.PHONY : CMakeFiles/GW.dir/clean

CMakeFiles/GW.dir/depend:
	cd /mnt/d/C/GW/cmake-build-release && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/d/C/GW /mnt/d/C/GW /mnt/d/C/GW/cmake-build-release /mnt/d/C/GW/cmake-build-release /mnt/d/C/GW/cmake-build-release/CMakeFiles/GW.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/GW.dir/depend

