cmake_minimum_required(VERSION 3.0.0)
project(lab4 VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(lab4 
    main.cpp 
    thread_pool/ThreadPool.cpp 
    thread_pool/ThreadExecutor.cpp 
    thread_pool/TaskGroup.cpp 
    array_operations.cpp
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_CXX_STANDARD 17)
include(CPack)
