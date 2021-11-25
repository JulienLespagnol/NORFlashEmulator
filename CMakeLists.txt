cmake_minimum_required(VERSION 3.0.0)
project(main VERSION 0.1.0)

# main target
set(SRCS_MAIN
main.c)

add_executable(main ${SRCS_MAIN})

# unit tests target
set(SRCS_TEST
nor_flash_emulator.c
test/main.c
munit/munit.c
test/init_tests.c
test/deinit_tests.c
test/read_tests.c)

set(HEADERS_TEST
munit/munit.h
nor_flash_emulator.h)

include_directories(test)
add_executable(test ${SRCS_TEST} ${HEADERS_TEST})