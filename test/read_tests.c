#include "../munit/munit.h"
#include "../nor_flash_emulator.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

nor_flash_emulator_handler *phandler_read = NULL;

void read_setup(void)
{
    uint32_t page_size = 256;
    uint32_t subsector_size = (page_size * 16);
    uint32_t sector_size = (subsector_size * 16);
    uint32_t flash_size = (sector_size * 127);

    nor_flash_emulator_params param =
        {
            .flash_size = flash_size,
            .page_size = page_size,
            .sector_size = sector_size,
            .subsector_size = subsector_size,
        };

    phandler_read = nor_flash_emulator_init(&param);
}

void read_teardown(void)
{
    nor_flash_emulator_deinit(&phandler_read);
}

MunitResult read_must_be_at_0xFF_after_init(const MunitParameter params[], void *user_data)
{
    int status = 0;
    uint8_t buff[1024];
    uint8_t check_buff[1024];

    memset(check_buff, 0xFF, 1024);

    status = nor_flash_emulator_read(phandler_read, 0, 1024, buff);

    munit_assert_int(status, ==, 1024);
    munit_assert_memory_equal(1024, buff, check_buff);

    return MUNIT_OK;
}

MunitResult read_must_return_error(const MunitParameter params[], void *user_data)
{
    int status = 0;
    uint8_t buff[1024];

    status = nor_flash_emulator_read(NULL, 0, 0, NULL);
    munit_assert_int(status, !=, 0);

    status = nor_flash_emulator_read(phandler_read, 0, 0, NULL);
    munit_assert_int(status, !=, 0);

    status = nor_flash_emulator_read(phandler_read, 0, 1, NULL);
    munit_assert_int(status, !=, 0);

    status = nor_flash_emulator_read(phandler_read, 0, (uint32_t)(phandler_read->params->flash_size + 1), buff);
    munit_assert_int(status, !=, 0);

    status = nor_flash_emulator_read(phandler_read, 1, (uint32_t)(phandler_read->params->flash_size), buff);
    munit_assert_int(status, !=, 0);

    return MUNIT_OK;
}

const MunitTest read_tests[] =
    {
        {
            "/must be at 0xFF after init",
            read_must_be_at_0xFF_after_init,
            read_setup,
            read_teardown,
            MUNIT_TEST_OPTION_NONE,
            NULL,
        },
        {
            "/must return error",
            read_must_return_error,
            read_setup,
            read_teardown,
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