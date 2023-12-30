CMAKE_MINIMUM_REQUIRED(VERSION 3.20)

project(RTL_TEST)
set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall -Wextra -ftemplate-backtrace-limit=0)
include_directories("../include")

enable_testing()
file(GLOB testFiles "./*.cpp")
foreach(file ${testFiles})
    get_filename_component(targetName ${file} NAME_WE)
    add_executable("${targetName}" ${file})
    add_test(NAME "${targetName}" COMMAND "${targetName}")
endforeach()

