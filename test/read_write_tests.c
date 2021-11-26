/**
 * @file read_write_test.c
 * @author Julien Lespagnol (lespagnolj@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-11-26
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "../munit/munit.h"
#include "../nor_flash_emulator.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "../munit/munit.h"
#include "../nor_flash_emulator.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define RW_TEST_BUFF_LEN 256
const uint8_t test_pattern[] = {0xDE, 0xAD, 0xBE, 0xEF};
uint8_t erased_value[RW_TEST_BUFF_LEN];
uint8_t test_value[RW_TEST_BUFF_LEN];

nor_flash_emulator_handler *phandler_rw = NULL;

void *rw_setup(const MunitParameter params[], void *user_data)
{
    uint32_t i = 0;
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

    phandler_rw = nor_flash_emulator_init(&param);

    memset(erased_value, 0xFF, RW_TEST_BUFF_LEN);

    for (i = 0; i < RW_TEST_BUFF_LEN; i += sizeof(test_pattern))
    {
        memcpy(test_value, test_pattern, sizeof(test_pattern));
    }
}

void rw_teardown(void *fixture)
{
    nor_flash_emulator_deinit(&phandler_rw);
}

MunitResult write_read_ok(const MunitParameter params[], void *user_data)
{
    uint8_t read_buffer[RW_TEST_BUFF_LEN];
    uint32_t i = 0;
    for (i = 0; i < phandler_rw->params->flash_size; i += RW_TEST_BUFF_LEN)
    {
        nor_flash_emulator_read(phandler_rw, i, RW_TEST_BUFF_LEN, read_buffer);
        munit_assert_memory_equal(RW_TEST_BUFF_LEN, read_buffer, erased_value);
    }

    return MUNIT_OK;
}

const MunitTest read_write_tests[] =
    {
        {
            "/ok",
            write_read_ok,
            rw_setup,
            rw_teardown,
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