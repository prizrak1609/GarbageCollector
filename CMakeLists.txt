cmake_minimum_required(VERSION 3.28)
project(garbageCollector)

set(CMAKE_CXX_STANDARD 17)

find_package(log4cplus REQUIRED)

add_executable(garbageCollector main.cpp
        GarbageCollectorReferenceCount.cpp
        GarbageCollectorReferenceCount.h)
target_link_libraries(garbageCollector log4cplus::log4cplus)
