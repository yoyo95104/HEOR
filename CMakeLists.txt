cmake_minimum_required(VERSION 3.10)
project(HEOR C CXX)  

set(CMAKE_CXX_STANDARD 11)  
set(CMAKE_C_STANDARD 99)     
set(CMAKE_C_FLAGS_RELEASE "-O3 -march=native -flto -DNDEBUG")
set(CMAKE_BUILD_TYPE Release)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
add_executable(HEOR main.c utl.c threads.c)

target_include_directories(HEOR PRIVATE ${GTK3_INCLUDE_DIRS})
target_link_libraries(HEOR ${GTK3_LIBRARIES} stdc++)
target_compile_options(HEOR PRIVATE ${GTK3_CFLAGS_OTHER})

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(HEOR PRIVATE -Wall -Wextra -Wpedantic)
endif()

