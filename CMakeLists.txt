cmake_minimum_required(VERSION 3.0.0)
project(main VERSION 0.1.0)

# main target
set(SRCS_MAIN
main.c)

add_executable(main ${SRCS_MAIN})

# unit tests target
set(SRCS_TEST
test/main.c)

include_directories(test)
add_executable(test ${SRCS_TEST})