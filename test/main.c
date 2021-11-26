/**
 * @file main.c
 * @author Julien Lespagnol (lespagnolj@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-11-26
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "../munit/munit.h"
#include <stdio.h>

extern const MunitTest init_tests[];
extern const MunitTest deinit_tests[];
extern const MunitTest read_tests[];
extern const MunitTest read_write_tests[];

MunitSuite suites[] = 
{
    {
    "init",
    (MunitTest *)init_tests,
    NULL,
    1,
    MUNIT_SUITE_OPTION_NONE,
    },

    {
    "deinit",
    (MunitTest *)deinit_tests,
    NULL,
    1,
    MUNIT_SUITE_OPTION_NONE,
    },

    {
    "read write",
    (MunitTest *)read_write_tests,
    NULL,
    1,
    MUNIT_SUITE_OPTION_NONE,
    },
    { NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, },
};

int main(void)
{
    munit_suite_main(suites, NULL, 0, NULL);
}