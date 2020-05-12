if (WIN32)
	# Small snippet from vcpkg integration guide
	# Allows for inclusion of vcpkg libraries
	if (DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
		set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

		# Set vcpkg triplet based upon generator
		if(CMAKE_SIZEOF_VOID_P EQUAL 8)
			set(VCPKG_TARGET_TRIPLET x64-windows)
		elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
			set(VCPKG_TARGET_TRIPED x86-windows)
		endif()
endif()
	endif()
endif()


# Project
cmake_minimum_required(VERSION 3.14)
project(simple-render VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release)


# Find packages
find_package(SDL2 2.0.6 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)


# Setup executable target
add_executable(simple-render src/simple-render.cpp)


# Set compiler options
target_compile_features(simple-render PRIVATE cxx_std_17)

if (MSVC)
	target_compile_options(simple-render PRIVATE /W4 /WX)
else()
	target_compile_options(simple-render PRIVATE -Wall -Wextra -pedantic -Werror)
endif()


# Add sources
target_include_directories(simple-render PRIVATE src)
# target_sources(simple-render PRIVATE)


# Link packages
target_link_libraries(simple-render PRIVATE SDL2::SDL2)
target_link_libraries(simple-render PRIVATE Vulkan::Vulkan)
target_link_libraries(simple-render PRIVATE fmt::fmt)
target_link_libraries(simple-render PRIVATE spdlog::spdlog)


if (WIN32)
	# Link SDL2main on windows to allow for a portable main()
	target_compile_definitions(simple-render PRIVATE SDL_MAIN_HANDLED)
	target_link_libraries(simple-render PRIVATE SDL2::SDL2main)
endif()