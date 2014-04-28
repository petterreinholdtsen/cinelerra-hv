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

# Utility rule file for pch_Generate_opencv_test_calib3d.

modules/calib3d/CMakeFiles/pch_Generate_opencv_test_calib3d: modules/calib3d/test_precomp.hpp.gch/opencv_test_calib3d_Release.gch

modules/calib3d/test_precomp.hpp.gch/opencv_test_calib3d_Release.gch: modules/calib3d/test/test_precomp.hpp
modules/calib3d/test_precomp.hpp.gch/opencv_test_calib3d_Release.gch: modules/calib3d/test_precomp.hpp
modules/calib3d/test_precomp.hpp.gch/opencv_test_calib3d_Release.gch: lib/libopencv_test_calib3d_pch_dephelp.a
	$(CMAKE_COMMAND) -E cmake_progress_report /root/hvirtual/thirdparty/OpenCV-2.3.1/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating test_precomp.hpp.gch/opencv_test_calib3d_Release.gch"
	cd /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d && /usr/bin/cmake -E make_directory /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d/test_precomp.hpp.gch
	cd /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d && /usr/bin/c++ -O3 -DNDEBUG -fomit-frame-pointer -msse -msse2 -DNDEBUG -I"/root/hvirtual/thirdparty/OpenCV-2.3.1/." -I"/root/hvirtual/thirdparty/OpenCV-2.3.1" -I"/root/hvirtual/thirdparty/OpenCV-2.3.1/include" -I"/root/hvirtual/thirdparty/OpenCV-2.3.1/include/opencv" -I"/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d/include" -I"/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d/src" -I"/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d" -I"/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d/../core/include" -I"/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d/../imgproc/include" -I"/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d/../highgui/include" -I"/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d/../features2d/include" -I"/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d/../flann/include" -I"/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d/test" -I"/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d/../calib3d/include" -I"/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d/../ts/include" -DHAVE_CVCONFIG_H -DCVAPI_EXPORTS -DHAVE_CVCONFIG_H -Wall -Wno-long-long -pthread -ffunction-sections -x c++-header -o /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d/test_precomp.hpp.gch/opencv_test_calib3d_Release.gch /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d/test_precomp.hpp

modules/calib3d/test_precomp.hpp: modules/calib3d/test/test_precomp.hpp
	$(CMAKE_COMMAND) -E cmake_progress_report /root/hvirtual/thirdparty/OpenCV-2.3.1/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating test_precomp.hpp"
	cd /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d && /usr/bin/cmake -E copy /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d/test/test_precomp.hpp /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d/test_precomp.hpp

pch_Generate_opencv_test_calib3d: modules/calib3d/CMakeFiles/pch_Generate_opencv_test_calib3d
pch_Generate_opencv_test_calib3d: modules/calib3d/test_precomp.hpp.gch/opencv_test_calib3d_Release.gch
pch_Generate_opencv_test_calib3d: modules/calib3d/test_precomp.hpp
pch_Generate_opencv_test_calib3d: modules/calib3d/CMakeFiles/pch_Generate_opencv_test_calib3d.dir/build.make
.PHONY : pch_Generate_opencv_test_calib3d

# Rule to build all files generated by this target.
modules/calib3d/CMakeFiles/pch_Generate_opencv_test_calib3d.dir/build: pch_Generate_opencv_test_calib3d
.PHONY : modules/calib3d/CMakeFiles/pch_Generate_opencv_test_calib3d.dir/build

modules/calib3d/CMakeFiles/pch_Generate_opencv_test_calib3d.dir/clean:
	cd /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d && $(CMAKE_COMMAND) -P CMakeFiles/pch_Generate_opencv_test_calib3d.dir/cmake_clean.cmake
.PHONY : modules/calib3d/CMakeFiles/pch_Generate_opencv_test_calib3d.dir/clean

modules/calib3d/CMakeFiles/pch_Generate_opencv_test_calib3d.dir/depend:
	cd /root/hvirtual/thirdparty/OpenCV-2.3.1 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/hvirtual/thirdparty/OpenCV-2.3.1 /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d /root/hvirtual/thirdparty/OpenCV-2.3.1 /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/calib3d/CMakeFiles/pch_Generate_opencv_test_calib3d.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : modules/calib3d/CMakeFiles/pch_Generate_opencv_test_calib3d.dir/depend

