#include "../munit/munit.h"
#include "../nor_flash_emulator.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

MunitResult init_must_return_null(const MunitParameter params[], void *user_data)
{
    nor_flash_emulator_handler *phandler = NULL;
    nor_flash_emulator_params param;

    /** test with null param **/
    phandler = nor_flash_emulator_init(NULL);

    munit_assert_null(phandler);

    /** test with 0 initialized param */
    memset(&param, 0, sizeof(nor_flash_emulator_params) / sizeof(uint8_t));
    phandler = nor_flash_emulator_init(&param);

    munit_assert_null(phandler);

    /** test with wrong configuration param */
    uint32_t flash_size = 1024;
    uint32_t sector_size = 1024;
    uint32_t subsector_size = 1024;
    uint32_t page_size = 1023;

    param.flash_size = flash_size;
    param.page_size = page_size;
    param.sector_size = sector_size;
    param.subsector_size = subsector_size;

    phandler = nor_flash_emulator_init(&param);
    munit_assert_null(phandler);
}

MunitResult init_must_not_return_null(const MunitParameter params[], void *user_data)
{
    uint32_t flash_size = 1024;
    uint32_t sector_size = 1024;
    uint32_t subsector_size = 1024;
    uint32_t page_size = 1024;

    nor_flash_emulator_handler *phandler = NULL;
    nor_flash_emulator_params param =
        {
            .flash_size = flash_size,
            .page_size = page_size,
            .sector_size = sector_size,
            .subsector_size = subsector_size,
        };

    phandler = nor_flash_emulator_init(&param);
    munit_assert_not_null(phandler);
    munit_assert_not_null(phandler->params);
    munit_assert_not_null(phandler->flash);

    return MUNIT_OK;
}

const MunitTest init_tests[] =
    {
        {
            "/test must return null",
            init_must_return_null,
            NULL,
            NULL,
            MUNIT_TEST_OPTION_NONE,
            NULL,
        },
        {
            "/test must not return null",
            init_must_not_return_null,
            NULL,
            NULL,
            MUNIT_TEST_OPTION_NONE,
            NULL,
        },
        {
            "",
            NULL,
            NULL,
            NULL,
            MUNIT_TEST_OPTION_NONE,
            NULL,
        },
};