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
CMAKE_SOURCE_DIR = /home/ubuntu/tiny-http-server

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ubuntu/tiny-http-server/build

# Include any dependencies generated for this target.
include src/CMakeFiles/server.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/server.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/server.dir/flags.make

src/CMakeFiles/server.dir/buffer.cpp.o: src/CMakeFiles/server.dir/flags.make
src/CMakeFiles/server.dir/buffer.cpp.o: ../src/buffer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubuntu/tiny-http-server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/server.dir/buffer.cpp.o"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/server.dir/buffer.cpp.o -c /home/ubuntu/tiny-http-server/src/buffer.cpp

src/CMakeFiles/server.dir/buffer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server.dir/buffer.cpp.i"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/tiny-http-server/src/buffer.cpp > CMakeFiles/server.dir/buffer.cpp.i

src/CMakeFiles/server.dir/buffer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server.dir/buffer.cpp.s"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/tiny-http-server/src/buffer.cpp -o CMakeFiles/server.dir/buffer.cpp.s

src/CMakeFiles/server.dir/epoller.cpp.o: src/CMakeFiles/server.dir/flags.make
src/CMakeFiles/server.dir/epoller.cpp.o: ../src/epoller.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubuntu/tiny-http-server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/CMakeFiles/server.dir/epoller.cpp.o"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/server.dir/epoller.cpp.o -c /home/ubuntu/tiny-http-server/src/epoller.cpp

src/CMakeFiles/server.dir/epoller.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server.dir/epoller.cpp.i"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/tiny-http-server/src/epoller.cpp > CMakeFiles/server.dir/epoller.cpp.i

src/CMakeFiles/server.dir/epoller.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server.dir/epoller.cpp.s"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/tiny-http-server/src/epoller.cpp -o CMakeFiles/server.dir/epoller.cpp.s

src/CMakeFiles/server.dir/httpconnect.cpp.o: src/CMakeFiles/server.dir/flags.make
src/CMakeFiles/server.dir/httpconnect.cpp.o: ../src/httpconnect.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubuntu/tiny-http-server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object src/CMakeFiles/server.dir/httpconnect.cpp.o"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/server.dir/httpconnect.cpp.o -c /home/ubuntu/tiny-http-server/src/httpconnect.cpp

src/CMakeFiles/server.dir/httpconnect.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server.dir/httpconnect.cpp.i"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/tiny-http-server/src/httpconnect.cpp > CMakeFiles/server.dir/httpconnect.cpp.i

src/CMakeFiles/server.dir/httpconnect.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server.dir/httpconnect.cpp.s"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/tiny-http-server/src/httpconnect.cpp -o CMakeFiles/server.dir/httpconnect.cpp.s

src/CMakeFiles/server.dir/httprequest.cpp.o: src/CMakeFiles/server.dir/flags.make
src/CMakeFiles/server.dir/httprequest.cpp.o: ../src/httprequest.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubuntu/tiny-http-server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object src/CMakeFiles/server.dir/httprequest.cpp.o"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/server.dir/httprequest.cpp.o -c /home/ubuntu/tiny-http-server/src/httprequest.cpp

src/CMakeFiles/server.dir/httprequest.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server.dir/httprequest.cpp.i"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/tiny-http-server/src/httprequest.cpp > CMakeFiles/server.dir/httprequest.cpp.i

src/CMakeFiles/server.dir/httprequest.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server.dir/httprequest.cpp.s"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/tiny-http-server/src/httprequest.cpp -o CMakeFiles/server.dir/httprequest.cpp.s

src/CMakeFiles/server.dir/httpresponse.cpp.o: src/CMakeFiles/server.dir/flags.make
src/CMakeFiles/server.dir/httpresponse.cpp.o: ../src/httpresponse.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubuntu/tiny-http-server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object src/CMakeFiles/server.dir/httpresponse.cpp.o"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/server.dir/httpresponse.cpp.o -c /home/ubuntu/tiny-http-server/src/httpresponse.cpp

src/CMakeFiles/server.dir/httpresponse.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server.dir/httpresponse.cpp.i"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/tiny-http-server/src/httpresponse.cpp > CMakeFiles/server.dir/httpresponse.cpp.i

src/CMakeFiles/server.dir/httpresponse.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server.dir/httpresponse.cpp.s"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/tiny-http-server/src/httpresponse.cpp -o CMakeFiles/server.dir/httpresponse.cpp.s

src/CMakeFiles/server.dir/server_main.cpp.o: src/CMakeFiles/server.dir/flags.make
src/CMakeFiles/server.dir/server_main.cpp.o: ../src/server_main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubuntu/tiny-http-server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object src/CMakeFiles/server.dir/server_main.cpp.o"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/server.dir/server_main.cpp.o -c /home/ubuntu/tiny-http-server/src/server_main.cpp

src/CMakeFiles/server.dir/server_main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server.dir/server_main.cpp.i"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/tiny-http-server/src/server_main.cpp > CMakeFiles/server.dir/server_main.cpp.i

src/CMakeFiles/server.dir/server_main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server.dir/server_main.cpp.s"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/tiny-http-server/src/server_main.cpp -o CMakeFiles/server.dir/server_main.cpp.s

src/CMakeFiles/server.dir/threadpoolV2.cpp.o: src/CMakeFiles/server.dir/flags.make
src/CMakeFiles/server.dir/threadpoolV2.cpp.o: ../src/threadpoolV2.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubuntu/tiny-http-server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object src/CMakeFiles/server.dir/threadpoolV2.cpp.o"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/server.dir/threadpoolV2.cpp.o -c /home/ubuntu/tiny-http-server/src/threadpoolV2.cpp

src/CMakeFiles/server.dir/threadpoolV2.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server.dir/threadpoolV2.cpp.i"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/tiny-http-server/src/threadpoolV2.cpp > CMakeFiles/server.dir/threadpoolV2.cpp.i

src/CMakeFiles/server.dir/threadpoolV2.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server.dir/threadpoolV2.cpp.s"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/tiny-http-server/src/threadpoolV2.cpp -o CMakeFiles/server.dir/threadpoolV2.cpp.s

src/CMakeFiles/server.dir/threadpoolV4.cpp.o: src/CMakeFiles/server.dir/flags.make
src/CMakeFiles/server.dir/threadpoolV4.cpp.o: ../src/threadpoolV4.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubuntu/tiny-http-server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object src/CMakeFiles/server.dir/threadpoolV4.cpp.o"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/server.dir/threadpoolV4.cpp.o -c /home/ubuntu/tiny-http-server/src/threadpoolV4.cpp

src/CMakeFiles/server.dir/threadpoolV4.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server.dir/threadpoolV4.cpp.i"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/tiny-http-server/src/threadpoolV4.cpp > CMakeFiles/server.dir/threadpoolV4.cpp.i

src/CMakeFiles/server.dir/threadpoolV4.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server.dir/threadpoolV4.cpp.s"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/tiny-http-server/src/threadpoolV4.cpp -o CMakeFiles/server.dir/threadpoolV4.cpp.s

src/CMakeFiles/server.dir/timer.cpp.o: src/CMakeFiles/server.dir/flags.make
src/CMakeFiles/server.dir/timer.cpp.o: ../src/timer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubuntu/tiny-http-server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object src/CMakeFiles/server.dir/timer.cpp.o"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/server.dir/timer.cpp.o -c /home/ubuntu/tiny-http-server/src/timer.cpp

src/CMakeFiles/server.dir/timer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server.dir/timer.cpp.i"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/tiny-http-server/src/timer.cpp > CMakeFiles/server.dir/timer.cpp.i

src/CMakeFiles/server.dir/timer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server.dir/timer.cpp.s"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/tiny-http-server/src/timer.cpp -o CMakeFiles/server.dir/timer.cpp.s

src/CMakeFiles/server.dir/webserver.cpp.o: src/CMakeFiles/server.dir/flags.make
src/CMakeFiles/server.dir/webserver.cpp.o: ../src/webserver.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubuntu/tiny-http-server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object src/CMakeFiles/server.dir/webserver.cpp.o"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/server.dir/webserver.cpp.o -c /home/ubuntu/tiny-http-server/src/webserver.cpp

src/CMakeFiles/server.dir/webserver.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server.dir/webserver.cpp.i"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/tiny-http-server/src/webserver.cpp > CMakeFiles/server.dir/webserver.cpp.i

src/CMakeFiles/server.dir/webserver.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server.dir/webserver.cpp.s"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/tiny-http-server/src/webserver.cpp -o CMakeFiles/server.dir/webserver.cpp.s

src/CMakeFiles/server.dir/db/database.cpp.o: src/CMakeFiles/server.dir/flags.make
src/CMakeFiles/server.dir/db/database.cpp.o: ../src/db/database.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubuntu/tiny-http-server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object src/CMakeFiles/server.dir/db/database.cpp.o"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/server.dir/db/database.cpp.o -c /home/ubuntu/tiny-http-server/src/db/database.cpp

src/CMakeFiles/server.dir/db/database.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server.dir/db/database.cpp.i"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/tiny-http-server/src/db/database.cpp > CMakeFiles/server.dir/db/database.cpp.i

src/CMakeFiles/server.dir/db/database.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server.dir/db/database.cpp.s"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/tiny-http-server/src/db/database.cpp -o CMakeFiles/server.dir/db/database.cpp.s

src/CMakeFiles/server.dir/model/usermodel.cpp.o: src/CMakeFiles/server.dir/flags.make
src/CMakeFiles/server.dir/model/usermodel.cpp.o: ../src/model/usermodel.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubuntu/tiny-http-server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building CXX object src/CMakeFiles/server.dir/model/usermodel.cpp.o"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/server.dir/model/usermodel.cpp.o -c /home/ubuntu/tiny-http-server/src/model/usermodel.cpp

src/CMakeFiles/server.dir/model/usermodel.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server.dir/model/usermodel.cpp.i"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/tiny-http-server/src/model/usermodel.cpp > CMakeFiles/server.dir/model/usermodel.cpp.i

src/CMakeFiles/server.dir/model/usermodel.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server.dir/model/usermodel.cpp.s"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/tiny-http-server/src/model/usermodel.cpp -o CMakeFiles/server.dir/model/usermodel.cpp.s

src/CMakeFiles/server.dir/log/log.cpp.o: src/CMakeFiles/server.dir/flags.make
src/CMakeFiles/server.dir/log/log.cpp.o: ../src/log/log.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubuntu/tiny-http-server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building CXX object src/CMakeFiles/server.dir/log/log.cpp.o"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/server.dir/log/log.cpp.o -c /home/ubuntu/tiny-http-server/src/log/log.cpp

src/CMakeFiles/server.dir/log/log.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server.dir/log/log.cpp.i"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/tiny-http-server/src/log/log.cpp > CMakeFiles/server.dir/log/log.cpp.i

src/CMakeFiles/server.dir/log/log.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server.dir/log/log.cpp.s"
	cd /home/ubuntu/tiny-http-server/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/tiny-http-server/src/log/log.cpp -o CMakeFiles/server.dir/log/log.cpp.s

# Object files for target server
server_OBJECTS = \
"CMakeFiles/server.dir/buffer.cpp.o" \
"CMakeFiles/server.dir/epoller.cpp.o" \
"CMakeFiles/server.dir/httpconnect.cpp.o" \
"CMakeFiles/server.dir/httprequest.cpp.o" \
"CMakeFiles/server.dir/httpresponse.cpp.o" \
"CMakeFiles/server.dir/server_main.cpp.o" \
"CMakeFiles/server.dir/threadpoolV2.cpp.o" \
"CMakeFiles/server.dir/threadpoolV4.cpp.o" \
"CMakeFiles/server.dir/timer.cpp.o" \
"CMakeFiles/server.dir/webserver.cpp.o" \
"CMakeFiles/server.dir/db/database.cpp.o" \
"CMakeFiles/server.dir/model/usermodel.cpp.o" \
"CMakeFiles/server.dir/log/log.cpp.o"

# External object files for target server
server_EXTERNAL_OBJECTS =

../bin/server: src/CMakeFiles/server.dir/buffer.cpp.o
../bin/server: src/CMakeFiles/server.dir/epoller.cpp.o
../bin/server: src/CMakeFiles/server.dir/httpconnect.cpp.o
../bin/server: src/CMakeFiles/server.dir/httprequest.cpp.o
../bin/server: src/CMakeFiles/server.dir/httpresponse.cpp.o
../bin/server: src/CMakeFiles/server.dir/server_main.cpp.o
../bin/server: src/CMakeFiles/server.dir/threadpoolV2.cpp.o
../bin/server: src/CMakeFiles/server.dir/threadpoolV4.cpp.o
../bin/server: src/CMakeFiles/server.dir/timer.cpp.o
../bin/server: src/CMakeFiles/server.dir/webserver.cpp.o
../bin/server: src/CMakeFiles/server.dir/db/database.cpp.o
../bin/server: src/CMakeFiles/server.dir/model/usermodel.cpp.o
../bin/server: src/CMakeFiles/server.dir/log/log.cpp.o
../bin/server: src/CMakeFiles/server.dir/build.make
../bin/server: src/CMakeFiles/server.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ubuntu/tiny-http-server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Linking CXX executable ../../bin/server"
	cd /home/ubuntu/tiny-http-server/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/server.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/server.dir/build: ../bin/server

.PHONY : src/CMakeFiles/server.dir/build

src/CMakeFiles/server.dir/clean:
	cd /home/ubuntu/tiny-http-server/build/src && $(CMAKE_COMMAND) -P CMakeFiles/server.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/server.dir/clean

src/CMakeFiles/server.dir/depend:
	cd /home/ubuntu/tiny-http-server/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ubuntu/tiny-http-server /home/ubuntu/tiny-http-server/src /home/ubuntu/tiny-http-server/build /home/ubuntu/tiny-http-server/build/src /home/ubuntu/tiny-http-server/build/src/CMakeFiles/server.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/server.dir/depend

