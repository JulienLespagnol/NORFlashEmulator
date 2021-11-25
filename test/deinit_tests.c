#include "../munit/munit.h"
#include "../nor_flash_emulator.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

MunitResult deinit_must_be_null(const MunitParameter params[], void *user_data)
{
    uint32_t page_size = 256;
    uint32_t subsector_size = (page_size * 16);
    uint32_t sector_size = (subsector_size * 16);
    uint32_t flash_size = (sector_size * 127);

    nor_flash_emulator_handler *phandler = NULL;
    nor_flash_emulator_params param =
        {
            .flash_size = flash_size,
            .page_size = page_size,
            .sector_size = sector_size,
            .subsector_size = subsector_size,
        };

    phandler = nor_flash_emulator_init(&param);
    nor_flash_emulator_deinit(&phandler);
 
    munit_assert_null(phandler);
}

const MunitTest deinit_tests[] =
    {
        {
            "/must be null",
            deinit_must_be_null,
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