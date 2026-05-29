cmake_minimum_required(VERSION 3.22)
project(mini_db)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src/storage)

add_executable(test_storage 
    test/test_storage.cpp 
    src/storage/StorageEngine.cpp 
    src/storage/Table.cpp
)