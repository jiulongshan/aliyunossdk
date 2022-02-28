#
# Copyright 2009-2017 Alibaba Cloud All rights reserved.
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#      http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.1)

cmake_policy(SET CMP0048 NEW)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#Project setting
file(STRINGS "VERSION" version)
project(alibabacloud-oss-cpp-sdk VERSION ${version})
message(STATUS "Project version: ${PROJECT_VERSION}")
set(SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(TARGET_OUTPUT_NAME_PREFIX "alibabacloud-oss-" CACHE STRING "The target's output name prefix")

#Options
option(BUILD_SHARED_LIBS  "Enable shared library" OFF)
option(BUILD_SAMPLE "Build sample" ON)
option(BUILD_TESTS "Build unit and perfermence tests" OFF)
option(ENABLE_COVERAGE "Flag to enable/disable building code with -fprofile-arcs and -ftest-coverage. Gcc only" OFF)
option(ENABLE_RTTI "Flag to enable/disable building code with RTTI information" ON)

set(TOOLCHAIN /media/lhb/long/Ingenic/sdk/mips-gcc472-glibc216-64bit/bin/
  CACHE STRING "Set toolchain path")
set(CROSS mips-linux-gnu-)
set(CMAKE_C_COMPILER ${TOOLCHAIN}${CROSS}gcc
  CACHE STRING "Set the cross-compiler tool CC" FORCE)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN}${CROSS}g++
  CACHE STRING "Set the cross-compiler tool CXX" FORCE)
set(CMAKE_LINKER ${TOOLCHAIN}${CROSS}ld
  CACHE STRING "Set the cross-compiler tool LD" FORCE)
set(CMAKE_AR ${TOOLCHAIN}${CROSS}ar
  CACHE STRING "Set the cross-compiler tool AR" FORCE)
set(CMAKE_NM ${TOOLCHAIN}${CROSS}nm
  CACHE STRING "Set the cross-compiler tool NM" FORCE)
set(CMAKE_OBJCOPY ${TOOLCHAIN}${CROSS}objcopy 
  CACHE STRING "Set the cross-compiler tool OBJCOPY" FORCE)
set(CMAKE_OBJDUMP ${TOOLCHAIN}${CROSS}objdump
  CACHE STRING "Set the cross-compiler tool OBJDUMP" FORCE)
set(CMAKE_RANLIB ${TOOLCHAIN}${CROSS}ranlib 
  CACHE STRING "Set the cross-compiler tool RANLIB" FORCE)
set(CMAKE_STRIP ${TOOLCHAIN}${CROSS}strip 
  CACHE STRING "Set the cross-compiler tool RANLIB" FORCE)

# set compile flags
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -march=mips32r2 -muclibc -EL -mmxu2 -flax-vector-conversions -std=gnu99")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -muclibc -EL -march=mips32r2 -mmxu2 -flax-vector-conversions -D_GLIBCXX_USE_NANOSLEEP")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -s -muclibc -std=gnu99")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -s -std=c++11 -muclibc")

#Platform
if (CMAKE_CROSSCOMPILING)
	if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
		set(PLATFORM_ANDROID 1)
		set(TARGET_OS "ANDROID")
	else()
		message(FATAL_ERROR "Do not support target platform")
	endif()
else()
	if(CMAKE_HOST_APPLE)
		set(PLATFORM_APPLE 1)
		set(TARGET_OS "APPLE")
	elseif(CMAKE_HOST_UNIX)
		set(PLATFORM_LINUX 1)
		set(TARGET_OS "LINUX")
	elseif(CMAKE_HOST_WIN32)
		set(PLATFORM_WINDOWS 1)
		set(TARGET_OS "WINDOWS")
	else()
		message(FATAL_ERROR "Do not support unknown host OS")
	endif()
endif()

message(STATUS "TARGET_OS: ${TARGET_OS}")

add_definitions(-DPLATFORM_${TARGET_OS})

#Find dependency Library, curl, openssl
if (${TARGET_OS} STREQUAL "WINDOWS")
	set(WLIB_TARGET "Win32")
	if (CMAKE_CL_64)
	set(WLIB_TARGET "x64")
	endif()
	set(CRYPTO_LIBS 
		${CMAKE_SOURCE_DIR}/third_party/lib/${WLIB_TARGET}/ssleay32.lib 
		${CMAKE_SOURCE_DIR}/third_party/lib/${WLIB_TARGET}/libeay32.lib)
	set(CRYPTO_INCLUDE_DIRS 
		${CMAKE_SOURCE_DIR}/third_party/include) 

	set(CLIENT_LIBS 
		${CMAKE_SOURCE_DIR}/third_party/lib/${WLIB_TARGET}/libcurl.lib) 
	set(CLIENT_INCLUDE_DIRS 
		${CMAKE_SOURCE_DIR}/third_party/include) 
else()
#[[
	include(FindCURL)
	include(FindOpenSSL)

	if(NOT CURL_FOUND)
		message(FATAL_ERROR "Could not find curl")
	endif()

	if(NOT OPENSSL_FOUND)
		message(FATAL_ERROR "Could not find openssl")
	endif()
#]]
	include_directories(
		/media/lhb/long/git/baby-monitor-main/3rd/curl/include
		/media/lhb/long/git/baby-monitor-main/3rd/libopenssl/include
	)

	link_directories(
		/media/lhb/long/git/baby-monitor-main/3rd/libopenssl/lib/
		/media/lhb/long/git/baby-monitor-main/3rd/curl/lib/
	)

	link_libraries(
		ssl
		crypto
		curl
	)

	set(CRYPTO_LIBS ${OPENSSL_LIBRARIES})
	set(CRYPTO_INCLUDE_DIRS ${OPENSSL_INCLUDE_DIRS}) 
	set(CRYPTO_LIBS_ABSTRACT_NAME crypto ssl)

	set(CLIENT_LIBS ${CURL_LIBRARIES})
	set(CLIENT_INCLUDE_DIRS ${CURL_INCLUDE_DIRS}) 
	set(CLIENT_LIBS_ABSTRACT_NAME curl)
endif()

add_definitions(-D_GLIBCXX_USE_C99=1)
add_definitions(-D_GLIBCXX_USE_NANOSLEEP)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -s -muclibc -std=gnu99")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -s -std=c++11 -muclibc")

#Compiler flags
list(APPEND SDK_COMPILER_FLAGS "-std=c++11")
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	list(APPEND SDK_COMPILER_FLAGS "/MP")
	if (NOT ENABLE_RTTI)
	list(APPEND SDK_COMPILER_FLAGS "/GR-")
	endif()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	list(APPEND SDK_COMPILER_FLAGS "-fno-exceptions" "-fPIC" "-fno-rtti")
	if (NOT ENABLE_RTTI)
	list(APPEND SDK_COMPILER_FLAGS "-fno-rtti")
	endif()
	
	#list(APPEND SDK_COMPILER_FLAGS "-Wall" "-Werror" "-pedantic" "-Wextra")
else()
	list(APPEND SDK_COMPILER_FLAGS "-fno-exceptions" "-fPIC")
	if (NOT ENABLE_RTTI)
	list(APPEND SDK_COMPILER_FLAGS "-fno-rtti")
	endif()
	
	#list(APPEND SDK_COMPILER_FLAGS "-Wall" "-Werror" "-pedantic" "-Wextra")
	
	if (ENABLE_COVERAGE)
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
	SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
	SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
	endif()
endif()


if (BUILD_SHARED_LIBS)
	set(STATIC_LIB_SUFFIX "-static")
else()
	set(STATIC_LIB_SUFFIX "")
endif()

include(ExternalProject)
include(GNUInstallDirs)

add_subdirectory(sdk)

if(BUILD_SAMPLE)
	add_subdirectory(sample)
endif()

if(BUILD_TESTS)
	add_subdirectory(test)
	add_subdirectory(ptest)
endif()
