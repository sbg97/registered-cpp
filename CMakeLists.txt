#Modified from template by Matt Morse
#https://github.com/mmorse1217/cmake-project-template

cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

project(registered-cpp VERSION 1.0)
set(registered-cpp_VERSION 1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)

# Function to prepend the subdirectory to source files in subdirectories
function(PREPEND var)
	set(listVar "")
	foreach(f ${${var}})
		list(APPEND listVar "${CMAKE_CURRENT_SOURCE_DIR}/${f}")
	endforeach(f)
	set(${var} "${listVar}" PARENT_SCOPE)
endfunction(PREPEND)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")

find_package(Boost REQUIRED fiber)

add_subdirectory(src)
add_subdirectory(include)

if(BUILD_TESTING)
	add_subdirectory(test)
endif()
add_library(registered-cpp STATIC ${registered-cpp_SRC} ${registered-cpp_INC})
target_compile_options(registered-cpp PRIVATE -Wall -Wextra -Werror)

set(registered-cpp_INCLUDE_DEST "include/registered-cpp-${registered-cpp_VERSION}")
set(registered-cpp_LIB_DEST "lib/registered-cpp-${registered-cpp_VERSION}")

target_include_directories(registered-cpp PUBLIC
	$<BUILD_INTERFACE:${registered-cpp_SOURCE_DIR}/include>
	$<BUILD_INTERFACE:${registered-cpp_BINARY_DIR}/include>
	
	$<INSTALL_INTERFACE:include>
)

include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(registered-cpp ${Boost_LIBRARIES})

install(
	TARGETS registered-cpp
	EXPORT registered-cppTargets
	LIBRARY DESTINATION ${registered-cpp_LIB_DEST}
	ARCHIVE DESTINATION ${registered-cpp_LIB_DEST}
	INCLUDES DESTINATION ${registered-cpp_INCLUDE_DEST}
)

install(
	EXPORT registered-cppTargets
	FILE registered-cppTargets.cmake
	NAMESPACE registered-cpp::
	DESTINATION $registered-cpp_LIB_DEST}
)

install(
	FILES ${registered-cpp_INC}
	DESTINATION ${registered-cpp_INCLUDE_DEST}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	registered-cppConfigVersion.cmake
	VERSION ${registered-cpp_VERSION}
	COMPATIBILITY ExactVersion
)

install(
	FILES
		"cmake/registered-cpp-config.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/registered-cppConfigVersion.cmake"
	DESTINATION ${registered-cpp_LIB_DEST}
)

