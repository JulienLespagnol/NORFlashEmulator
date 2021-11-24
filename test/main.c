#include "../munit/munit.h"
#include <stdio.h>

extern const MunitTest init_tests[];

MunitSuite suites[] = 
{
    {
    "init",
    init_tests,
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