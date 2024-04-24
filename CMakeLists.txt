cmake_minimum_required(VERSION 3.10)
project(RootIVAnalyser VERSION 1.0)

# Add C++11 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the source and header files for your library
file(GLOB SOURCES "source/*.cpp")
file(GLOB HEADERS "include/*.hpp")

# Create a shared library
add_library(RootIVAnalyser SHARED ${SOURCES} ${HEADERS})

# Locate Root libraries
find_package(ROOT REQUIRED COMPONENTS RIO Hist Core Graf Graf3d Tree)

include_directories(${ROOT_INCLUDE_DIRS})

# Link with Root libraries
target_link_libraries(RootIVAnalyser PRIVATE ${ROOT_LIBRARIES})

# Set the output name of the library (optional)
# By default, it will be "libRootIVAnalyser.so" on Linux,
# "RootIVAnalyser.dll" on Windows, and "libRootIVAnalyser.dylib" on macOS.
# Uncomment and modify if you want a different name
# set_target_properties(RootIVAnalyser PROPERTIES OUTPUT_NAME "RootIVAnalyser")

# Include the current directory for header files
target_include_directories(RootIVAnalyser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Set installation directories (optional)
# By default, it will install in the system directories
# Modify as needed
# For example:
# set(INSTALL_LIB_DIR lib)
# set(INSTALL_INCLUDE_DIR include)
# install(TARGETS RootIVAnalyser DESTINATION ${INSTALL_LIB_DIR})
# install(FILES ${HEADERS} DESTINATION ${INSTALL_INCLUDE_DIR})

# Optionally, you can set additional compiler flags
# For example, to enable debug symbols
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")