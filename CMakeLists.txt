cmake_minimum_required(VERSION 3.0)
project(tp_async)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_RELEASE} -O2")

add_executable(test_quicksort test_quicksort.cc)
target_link_libraries(test_quicksort pthread)

add_executable(test_async test_async.cc)
target_link_libraries(test_async pthread)
