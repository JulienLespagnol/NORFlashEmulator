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
#include "munit/munit.h"
#include "nor_flash_emulator.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/************************* INIT TESTS *******************************************/

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
    munit_assert_not_null(phandler);
    munit_assert_not_null(phandler->params);
    munit_assert_not_null(phandler->flash);

    return MUNIT_OK;
}

MunitResult test_deinit(const MunitParameter params[], void *user_data)
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
            "/deinit",
            test_deinit,
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

/************************* R/W/ERASE TESTS *******************************************/
#define RW_TEST_BUFF_LEN 256
const uint8_t test_pattern[] = {0xDE, 0xAD, 0xBE, 0xEF};
uint8_t erased_value[RW_TEST_BUFF_LEN];
uint8_t test_value[RW_TEST_BUFF_LEN];
uint8_t zero_value[RW_TEST_BUFF_LEN];

#define WRITE_TEST_VALUE_1 0xAA
#define WRITE_TEST_VALUE_2 0x55

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
    memset(zero_value, 0x00, RW_TEST_BUFF_LEN);

    for (i = 0; i < RW_TEST_BUFF_LEN; i += sizeof(test_pattern))
    {
        memcpy(test_value, test_pattern, sizeof(test_pattern));
    }
}

void rw_teardown(void *fixture)
{
    nor_flash_emulator_deinit(&phandler_rw);
}

MunitResult write_read_ok_after_init(const MunitParameter params[], void *user_data)
{
    uint8_t read_buffer[RW_TEST_BUFF_LEN];
    uint32_t i = 0;
    int status = 0;
    FILE *file = NULL;

    for (i = 0; i < phandler_rw->params->flash_size; i += RW_TEST_BUFF_LEN)
    {
        memset(read_buffer, 0, RW_TEST_BUFF_LEN);
        status = nor_flash_emulator_read(phandler_rw, i, RW_TEST_BUFF_LEN, read_buffer);
        munit_assert_memory_equal(RW_TEST_BUFF_LEN, read_buffer, erased_value);
        munit_assert_int(status, ==, RW_TEST_BUFF_LEN);
    }

    file = fopen("./save.bin", "wb+");
    status = nor_flash_emulator_save_to_file(phandler_rw, file);
    fclose(file);

    file = fopen("./save.bin", "rb");

    for (i = 0; i < phandler_rw->params->flash_size; i += RW_TEST_BUFF_LEN)
    {
        memset(read_buffer, 0, RW_TEST_BUFF_LEN);
        status = fread(read_buffer, sizeof(uint8_t), RW_TEST_BUFF_LEN, file);
        munit_assert_memory_equal(RW_TEST_BUFF_LEN, read_buffer, erased_value);
        munit_assert_int(status, ==, RW_TEST_BUFF_LEN);
    }

    fclose(file);

    return MUNIT_OK;
}

MunitResult write_read_ok_write_and_check(const MunitParameter params[], void *user_data)
{
    uint8_t read_buffer[RW_TEST_BUFF_LEN];
    uint32_t i = 0;
    int status = 0;
    FILE *file = NULL;

    for (i = 0; i < phandler_rw->params->flash_size; i += RW_TEST_BUFF_LEN)
    {
        status = nor_flash_emulator_write(phandler_rw, i, RW_TEST_BUFF_LEN, test_value);
        munit_assert_int(status, ==, RW_TEST_BUFF_LEN);
    }

    for (i = 0; i < phandler_rw->params->flash_size; i += RW_TEST_BUFF_LEN)
    {
        memset(read_buffer, 0, RW_TEST_BUFF_LEN);
        status = nor_flash_emulator_read(phandler_rw, i, RW_TEST_BUFF_LEN, read_buffer);
        munit_assert_memory_equal(RW_TEST_BUFF_LEN, read_buffer, test_value);
        munit_assert_int(status, ==, RW_TEST_BUFF_LEN);
    }

    file = fopen("./save.bin", "wb+");
    status = nor_flash_emulator_save_to_file(phandler_rw, file);
    fclose(file);

    for (i = 0; i < phandler_rw->params->flash_size; i += RW_TEST_BUFF_LEN)
    {
        memset(read_buffer, 0, RW_TEST_BUFF_LEN);
        status = fread(read_buffer, sizeof(uint8_t), RW_TEST_BUFF_LEN, file);
        munit_assert_memory_equal(RW_TEST_BUFF_LEN, read_buffer, test_value);
        munit_assert_int(status, ==, RW_TEST_BUFF_LEN);
    }

    file = fopen("./save.bin", "rb");
    fclose(file);
    return MUNIT_OK;
}

MunitResult write_read_ok_read_too_long(const MunitParameter params[], void *user_data)
{
    uint8_t read_buffer[RW_TEST_BUFF_LEN];
    uint32_t i = 0;
    int status = 0;
    uint32_t test_len = 16;

    status = nor_flash_emulator_read(phandler_rw, phandler_rw->params->flash_size - test_len, RW_TEST_BUFF_LEN, read_buffer);
    munit_assert_int(status, ==, test_len);

    return MUNIT_OK;
}

MunitResult write_read_ok_read_address_out_of_flash(const MunitParameter params[], void *user_data)
{
    uint8_t read_buffer[RW_TEST_BUFF_LEN];
    uint32_t i = 0;
    int status = 0;
    uint32_t test_len = 16;

    status = nor_flash_emulator_read(phandler_rw, phandler_rw->params->flash_size + 1, RW_TEST_BUFF_LEN, read_buffer);
    munit_assert_int(status, ==, -1);

    return MUNIT_OK;
}

MunitResult write_read_ok_write_same_bytes(const MunitParameter params[], void *user_data)
{
    uint8_t read_buffer[RW_TEST_BUFF_LEN];
    uint8_t test_value_1 = WRITE_TEST_VALUE_1;
    uint8_t test_value_2 = WRITE_TEST_VALUE_2;
    uint32_t i = 0;
    int status = 0;

    for (i = 0; i < phandler_rw->params->flash_size; i++)
    {
        status = nor_flash_emulator_write(phandler_rw, i, 1, &test_value_1);
        munit_assert_int(status, ==, 1);

        status = nor_flash_emulator_write(phandler_rw, i, 1, &test_value_2);
        munit_assert_int(status, ==, 1);
    }

    for (i = 0; i < phandler_rw->params->flash_size; i += RW_TEST_BUFF_LEN)
    {
        memset(read_buffer, 0, RW_TEST_BUFF_LEN);
        status = nor_flash_emulator_read(phandler_rw, i, RW_TEST_BUFF_LEN, read_buffer);
        munit_assert_memory_equal(RW_TEST_BUFF_LEN, read_buffer, zero_value);
        munit_assert_int(status, ==, RW_TEST_BUFF_LEN);
    }

    return MUNIT_OK;
}

MunitResult write_read_ok_write_and_erase_subsector(const MunitParameter params[], void *user_data)
{
    uint32_t subsector_nb = 0;
    uint8_t read_buffer[RW_TEST_BUFF_LEN];
    uint32_t i = 0;
    int status = 0;

    for (i = 0; i < phandler_rw->params->flash_size; i += RW_TEST_BUFF_LEN)
    {
        status = nor_flash_emulator_write(phandler_rw, i, RW_TEST_BUFF_LEN, test_value);
        munit_assert_int(status, ==, RW_TEST_BUFF_LEN);
    }

    for (i = 0; i < phandler_rw->params->flash_size; i += phandler_rw->params->subsector_size)
    {
        status = nor_flash_emulator_erase_subsector(phandler_rw, subsector_nb);
        munit_assert_int(status, ==, 0);
        subsector_nb++;
    }

    for (i = 0; i < phandler_rw->params->flash_size; i += RW_TEST_BUFF_LEN)
    {
        memset(read_buffer, 0, RW_TEST_BUFF_LEN);
        status = nor_flash_emulator_read(phandler_rw, i, RW_TEST_BUFF_LEN, read_buffer);
        munit_assert_memory_equal(RW_TEST_BUFF_LEN, read_buffer, erased_value);
        munit_assert_int(status, ==, RW_TEST_BUFF_LEN);
    }

    return MUNIT_OK;
}

MunitResult write_read_ok_write_and_erase_sector(const MunitParameter params[], void *user_data)
{
    uint32_t sector_nb = 0;
    uint8_t read_buffer[RW_TEST_BUFF_LEN];
    uint32_t i = 0;
    int status = 0;

    for (i = 0; i < phandler_rw->params->flash_size; i += RW_TEST_BUFF_LEN)
    {
        status = nor_flash_emulator_write(phandler_rw, i, RW_TEST_BUFF_LEN, test_value);
        munit_assert_int(status, ==, RW_TEST_BUFF_LEN);
    }

    for (i = 0; i < phandler_rw->params->flash_size; i += phandler_rw->params->sector_size)
    {
        status = nor_flash_emulator_erase_sector(phandler_rw, sector_nb);
        munit_assert_int(status, ==, 0);
        sector_nb++;
    }

    for (i = 0; i < phandler_rw->params->flash_size; i += RW_TEST_BUFF_LEN)
    {
        memset(read_buffer, 0, RW_TEST_BUFF_LEN);
        status = nor_flash_emulator_read(phandler_rw, i, RW_TEST_BUFF_LEN, read_buffer);
        munit_assert_memory_equal(RW_TEST_BUFF_LEN, read_buffer, erased_value);
        munit_assert_int(status, ==, RW_TEST_BUFF_LEN);
    }

    return MUNIT_OK;
}

MunitResult write_read_ok_write_and_bulk_erase(const MunitParameter params[], void *user_data)
{
    uint8_t read_buffer[RW_TEST_BUFF_LEN];
    uint32_t i = 0;
    int status = 0;

    for (i = 0; i < phandler_rw->params->flash_size; i += RW_TEST_BUFF_LEN)
    {
        status = nor_flash_emulator_write(phandler_rw, i, RW_TEST_BUFF_LEN, test_value);
        munit_assert_int(status, ==, RW_TEST_BUFF_LEN);
    }

    nor_flash_emulator_bulk_erase(phandler_rw);

    for (i = 0; i < phandler_rw->params->flash_size; i += RW_TEST_BUFF_LEN)
    {
        memset(read_buffer, 0, RW_TEST_BUFF_LEN);
        status = nor_flash_emulator_read(phandler_rw, i, RW_TEST_BUFF_LEN, read_buffer);
        munit_assert_memory_equal(RW_TEST_BUFF_LEN, read_buffer, erased_value);
        munit_assert_int(status, ==, RW_TEST_BUFF_LEN);
    }

    return MUNIT_OK;
}

const MunitTest read_write_tests[] =
    {
        {
            "/after init",
            write_read_ok_after_init,
            rw_setup,
            rw_teardown,
            MUNIT_TEST_OPTION_NONE,
            NULL,
        },
        {
            "/write and check",
            write_read_ok_after_init,
            rw_setup,
            rw_teardown,
            MUNIT_TEST_OPTION_NONE,
            NULL,
        },
        {
            "/read too long",
            write_read_ok_read_too_long,
            rw_setup,
            rw_teardown,
            MUNIT_TEST_OPTION_NONE,
            NULL,
        },
        {
            "/read out of flash",
            write_read_ok_read_address_out_of_flash,
            rw_setup,
            rw_teardown,
            MUNIT_TEST_OPTION_NONE,
            NULL,
        },
        {
            "/rewrite same bytes",
            write_read_ok_write_same_bytes,
            rw_setup,
            rw_teardown,
            MUNIT_TEST_OPTION_NONE,
            NULL,
        },
        {
            "/write and erase subsector",
            write_read_ok_write_and_erase_subsector,
            rw_setup,
            rw_teardown,
            MUNIT_TEST_OPTION_NONE,
            NULL,
        },
        {
            "/write and erase sector",
            write_read_ok_write_and_erase_sector,
            rw_setup,
            rw_teardown,
            MUNIT_TEST_OPTION_NONE,
            NULL,
        },
        {
            "/write and erase bulk erase",
            write_read_ok_write_and_bulk_erase,
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
            "r/w erase",
            (MunitTest *)read_write_tests,
            NULL,
            1,
            MUNIT_SUITE_OPTION_NONE,
        },
        {
            NULL,
            NULL,
            NULL,
            MUNIT_TEST_OPTION_NONE,
        },
};

int main(void)
{
    munit_suite_main(suites, NULL, 0, NULL);
}