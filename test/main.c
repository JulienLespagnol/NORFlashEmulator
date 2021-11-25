#include "../munit/munit.h"
#include <stdio.h>

extern const MunitTest init_tests[];
extern const MunitTest deinit_tests[];
extern const MunitTest read_tests[];

MunitSuite suites[] = 
{
    {
    "init",
    init_tests,
    NULL,
    1,
    MUNIT_SUITE_OPTION_NONE,
    },

    {
    "deinit",
    deinit_tests,
    NULL,
    1,
    MUNIT_SUITE_OPTION_NONE,
    },

    {
    "read",
    read_tests,
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