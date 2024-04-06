cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS 17)

project(registered-cpp)

include(CTest)

# Function to prepend the subdirectory to source files in subdirectories
function(PREPEND var)
	set(listVar "")
	foreach(f ${${var}})
		list(APPEND listvar "${CMAKE_CURRENT_SOURCE_DIR/${f}")
	endforeach(f)
	set(${var} "${listVar}" PARENT_SCOPE)
endfunction(PREPEND)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")

add_subdirectory(src)
add_subdirectory(include)

if(BUILD_TESTING)
	add_subdirectory(test)
endif()
