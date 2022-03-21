cmake_minimum_required(VERSION 3.2)

# Создание проекта
project(laba1)

# Установка переменных
# set(SOURCE_EXE main.cpp)
set(SOURCE_BINARY "lab_01")

find_package(GTest REQUIRED)
find_package(Threads)
find_package(ZLIB)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL Data)

if(NOT ${Poco_FOUND})
    message(FATAL_ERROR "Poco C++ Libraries not found.")
endif()

include_directories(${Poco_INCLUDE_DIRS})

# Добавление исполняемых файлов 
add_executable(gtests test.cpp)
add_executable(${SOURCE_BINARY} main.cpp)

add_executable(WebServer WebServer.cpp)

# add_executable(laba_exe ${SOURCE_EXE})
add_library(library config.cpp)

link_directories("/usr/local/lib")


target_include_directories(${SOURCE_BINARY} PRIVATE "$CMAKE_BINARY_DIR")
target_include_directories(library PRIVATE "$CMAKE_BINARY_DIR")
target_include_directories(gtests PRIVATE "$CMAKE_BINARY_DIR")
target_include_directories(WebServer PRIVATE "$CMAKE_BINARY_DIR")

target_compile_options(${SOURCE_BINARY} PRIVATE -Wall -Wextra -pedantic -Werror)
target_compile_options(library PRIVATE -Wall -Wextra -pedantic -Werror)
target_compile_options(gtests PRIVATE -Wall -Wextra -pedantic -Werror)

target_compile_options(WebServer PRIVATE -Wall -Wextra -pedantic -Werror )

target_link_libraries(${SOURCE_BINARY} PRIVATE library ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(gtests PRIVATE library ${GTEST_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

target_link_libraries(WebServer PRIVATE 
                        ${CMAKE_THREAD_LIBS_INIT} 
                        ${Poco_LIBRARIES} 
                        "PocoDataMySQL"
                        ZLIB::ZLIB)

set_target_properties(WebServer
                     PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

enable_testing()
add_test(gtests gtests)