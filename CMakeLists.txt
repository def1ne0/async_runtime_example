cmake_minimum_required(VERSION 4.2)
project(async_runtime_example)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-fsanitize=address,undefined,leak)
add_link_options(-fsanitize=address,undefined,leak)

find_package(Boost REQUIRED COMPONENTS atomic)

add_executable(${PROJECT_NAME} main.cpp)

target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET modules
        TYPE CXX_MODULES FILES
        include/coro/awaiters/async_sleep.cppm
        include/coro/concepts/pool_concept.cppm
        include/coro/concepts/task_concept.cppm
        include/coro/pool/event_pool.cppm
        include/coro/task/task.cppm
)

target_sources(${PROJECT_NAME} PRIVATE
        src/coro/pool/event_pool.cpp
        src/coro/awaiters/async_sleep.cpp
)


target_link_libraries(${PROJECT_NAME} PRIVATE Boost::atomic atomic)
