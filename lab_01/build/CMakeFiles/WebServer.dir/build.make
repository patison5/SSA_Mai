# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_SOURCE_DIR = /home/stud/src/SSA_Mai/lab_01

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/stud/src/SSA_Mai/lab_01/build

# Include any dependencies generated for this target.
include CMakeFiles/WebServer.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/WebServer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/WebServer.dir/flags.make

CMakeFiles/WebServer.dir/WebServer.cpp.o: CMakeFiles/WebServer.dir/flags.make
CMakeFiles/WebServer.dir/WebServer.cpp.o: ../WebServer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stud/src/SSA_Mai/lab_01/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/WebServer.dir/WebServer.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/WebServer.dir/WebServer.cpp.o -c /home/stud/src/SSA_Mai/lab_01/WebServer.cpp

CMakeFiles/WebServer.dir/WebServer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WebServer.dir/WebServer.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/stud/src/SSA_Mai/lab_01/WebServer.cpp > CMakeFiles/WebServer.dir/WebServer.cpp.i

CMakeFiles/WebServer.dir/WebServer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WebServer.dir/WebServer.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/stud/src/SSA_Mai/lab_01/WebServer.cpp -o CMakeFiles/WebServer.dir/WebServer.cpp.s

# Object files for target WebServer
WebServer_OBJECTS = \
"CMakeFiles/WebServer.dir/WebServer.cpp.o"

# External object files for target WebServer
WebServer_EXTERNAL_OBJECTS =

WebServer: CMakeFiles/WebServer.dir/WebServer.cpp.o
WebServer: CMakeFiles/WebServer.dir/build.make
WebServer: /usr/local/lib/libPocoNetSSL.so.81
WebServer: /usr/local/lib/libPocoData.so.81
WebServer: /usr/lib/x86_64-linux-gnu/libz.so
WebServer: /usr/local/lib/libPocoUtil.so.81
WebServer: /usr/local/lib/libPocoXML.so.81
WebServer: /usr/local/lib/libPocoJSON.so.81
WebServer: /usr/local/lib/libPocoNet.so.81
WebServer: /usr/local/lib/libPocoCrypto.so.81
WebServer: /usr/lib/x86_64-linux-gnu/libssl.so
WebServer: /usr/lib/x86_64-linux-gnu/libcrypto.so
WebServer: /usr/local/lib/libPocoFoundation.so.81
WebServer: CMakeFiles/WebServer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/stud/src/SSA_Mai/lab_01/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable WebServer"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/WebServer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/WebServer.dir/build: WebServer

.PHONY : CMakeFiles/WebServer.dir/build

CMakeFiles/WebServer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/WebServer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/WebServer.dir/clean

CMakeFiles/WebServer.dir/depend:
	cd /home/stud/src/SSA_Mai/lab_01/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/stud/src/SSA_Mai/lab_01 /home/stud/src/SSA_Mai/lab_01 /home/stud/src/SSA_Mai/lab_01/build /home/stud/src/SSA_Mai/lab_01/build /home/stud/src/SSA_Mai/lab_01/build/CMakeFiles/WebServer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/WebServer.dir/depend

