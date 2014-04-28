# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
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

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /root/hvirtual/thirdparty/OpenCV-2.3.1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/hvirtual/thirdparty/OpenCV-2.3.1

# Include any dependencies generated for this target.
include modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/depend.make

# Include the progress variables for this target.
include modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/progress.make

# Include the compile flags for this target's objects.
include modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/flags.make

modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/opencv_calib3d_pch_dephelp.o: modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/flags.make
modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/opencv_calib3d_pch_dephelp.o: modules/calib3d/opencv_calib3d_pch_dephelp.cxx
	$(CMAKE_COMMAND) -E cmake_progress_report /root/hvirtual/thirdparty/OpenCV-2.3.1/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/opencv_calib3d_pch_dephelp.o"
	cd /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/opencv_calib3d_pch_dephelp.dir/opencv_calib3d_pch_dephelp.o -c /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d/opencv_calib3d_pch_dephelp.cxx

modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/opencv_calib3d_pch_dephelp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opencv_calib3d_pch_dephelp.dir/opencv_calib3d_pch_dephelp.i"
	cd /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d/opencv_calib3d_pch_dephelp.cxx > CMakeFiles/opencv_calib3d_pch_dephelp.dir/opencv_calib3d_pch_dephelp.i

modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/opencv_calib3d_pch_dephelp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opencv_calib3d_pch_dephelp.dir/opencv_calib3d_pch_dephelp.s"
	cd /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d/opencv_calib3d_pch_dephelp.cxx -o CMakeFiles/opencv_calib3d_pch_dephelp.dir/opencv_calib3d_pch_dephelp.s

modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/opencv_calib3d_pch_dephelp.o.requires:
.PHONY : modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/opencv_calib3d_pch_dephelp.o.requires

modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/opencv_calib3d_pch_dephelp.o.provides: modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/opencv_calib3d_pch_dephelp.o.requires
	$(MAKE) -f modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/build.make modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/opencv_calib3d_pch_dephelp.o.provides.build
.PHONY : modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/opencv_calib3d_pch_dephelp.o.provides

modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/opencv_calib3d_pch_dephelp.o.provides.build: modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/opencv_calib3d_pch_dephelp.o
.PHONY : modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/opencv_calib3d_pch_dephelp.o.provides.build

modules/calib3d/opencv_calib3d_pch_dephelp.cxx: modules/calib3d/src/precomp.hpp
	$(CMAKE_COMMAND) -E cmake_progress_report /root/hvirtual/thirdparty/OpenCV-2.3.1/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating opencv_calib3d_pch_dephelp.cxx"
	cd /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d && /usr/bin/cmake -E echo \#include\ \"/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d/src/precomp.hpp\" > /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d/opencv_calib3d_pch_dephelp.cxx
	cd /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d && /usr/bin/cmake -E echo int\ testfunction\(\) >> /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d/opencv_calib3d_pch_dephelp.cxx
	cd /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d && /usr/bin/cmake -E echo { >> /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d/opencv_calib3d_pch_dephelp.cxx
	cd /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d && /usr/bin/cmake -E echo \ \ \ \ \return\ 0\; >> /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d/opencv_calib3d_pch_dephelp.cxx
	cd /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d && /usr/bin/cmake -E echo } >> /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d/opencv_calib3d_pch_dephelp.cxx

# Object files for target opencv_calib3d_pch_dephelp
opencv_calib3d_pch_dephelp_OBJECTS = \
"CMakeFiles/opencv_calib3d_pch_dephelp.dir/opencv_calib3d_pch_dephelp.o"

# External object files for target opencv_calib3d_pch_dephelp
opencv_calib3d_pch_dephelp_EXTERNAL_OBJECTS =

lib/libopencv_calib3d_pch_dephelp.a: modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/opencv_calib3d_pch_dephelp.o
lib/libopencv_calib3d_pch_dephelp.a: modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/build.make
lib/libopencv_calib3d_pch_dephelp.a: modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX static library ../../lib/libopencv_calib3d_pch_dephelp.a"
	cd /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d && $(CMAKE_COMMAND) -P CMakeFiles/opencv_calib3d_pch_dephelp.dir/cmake_clean_target.cmake
	cd /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/opencv_calib3d_pch_dephelp.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/build: lib/libopencv_calib3d_pch_dephelp.a
.PHONY : modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/build

modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/requires: modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/opencv_calib3d_pch_dephelp.o.requires
.PHONY : modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/requires

modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/clean:
	cd /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d && $(CMAKE_COMMAND) -P CMakeFiles/opencv_calib3d_pch_dephelp.dir/cmake_clean.cmake
.PHONY : modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/clean

modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/depend: modules/calib3d/opencv_calib3d_pch_dephelp.cxx
	cd /root/hvirtual/thirdparty/OpenCV-2.3.1 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/hvirtual/thirdparty/OpenCV-2.3.1 /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d /root/hvirtual/thirdparty/OpenCV-2.3.1 /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : modules/calib3d/CMakeFiles/opencv_calib3d_pch_dephelp.dir/depend

