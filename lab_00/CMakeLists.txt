cmake_minimum_required(VERSION 3.2)

# Создание проекта
project(laba1)

# Установка переменных
# set(SOURCE_EXE main.cpp)
set(SOURCE_BINARY "lab_01")

find_package(GTest REQUIRED)
find_package(Threads)

# Добавление исполняемых файлов 
add_executable(gtests test.cpp)
add_executable(${SOURCE_BINARY} main.cpp)
# add_executable(laba_exe ${SOURCE_EXE})
add_library(library config.cpp)

target_include_directories(${SOURCE_BINARY} PRIVATE "$CMAKE_BINARY_DIR")
target_include_directories(library PRIVATE "$CMAKE_BINARY_DIR")
target_include_directories(gtests PRIVATE "$CMAKE_BINARY_DIR")

target_compile_options(${SOURCE_BINARY} PRIVATE -Wall -Wextra -pedantic -Werror)
target_compile_options(library PRIVATE -Wall -Wextra -pedantic -Werror)
target_compile_options(gtests PRIVATE -Wall -Wextra -pedantic -Werror)

target_link_libraries(${SOURCE_BINARY} PRIVATE library ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(gtests PRIVATE library ${GTEST_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

enable_testing()
add_test(gtests gtests)