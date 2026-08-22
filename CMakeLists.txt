cmake_minimum_required(VERSION 3.28)
project(fastreads)

set(CMAKE_CXX_STANDARD 17)

#configure_file(
#        "${CMAKE_CURRENT_SOURCE_DIR}/tenmil.csv"
#        "${CMAKE_CURRENT_BINARY_DIR}/tenmil.csv"
#        COPYONLY
#)

#configure_file(
#        "${CMAKE_CURRENT_SOURCE_DIR}/100mil.csv"
#        "${CMAKE_CURRENT_BINARY_DIR}/100mil.csv"
#        COPYONLY
#)

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/measurements.csv"
        "${CMAKE_CURRENT_BINARY_DIR}/measurements.csv"
        COPYONLY
)

find_package(absl REQUIRED)

add_subdirectory(benchmark)

add_executable(fastreads
        main.cpp
        flatmap.h)

target_link_libraries(fastreads PRIVATE absl::flat_hash_map benchmark::benchmark)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native -mtune=native")
