/**
 * @file nor_flash_emulator.c
 * @author Julien Lespagnol (lespagnolj@gmail.com)
 * @brief
 * @version 0.1
 * @date 2021-11-11
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "nor_flash_emulator.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct nor_flash_emulator_flash_t
{
    uint8_t *buffer;

    nor_flash_emulator_params *params;

    uint32_t number_of_sector;
    uint32_t number_of_subtector_per_sector;
    uint32_t number_of_page_per_subsector;

    uint32_t total_number_of_subsector;
    uint32_t total_number_of_page;

    bool is_init;

} nor_flash_emulator_flash;

/**
 * @brief 
 * 
 * @param pflash 
 * @param params 
 * @return int 
 */
static int nor_flash_emulator_flash_init(nor_flash_emulator_flash *pflash, nor_flash_emulator_params *params)
{
    int status = -1;

    if ((params->flash_size != 0) &&
        (params->page_size != 0) &&
        (params->sector_size != 0) &&
        (params->subsector_size != 0))
    {
        pflash->params = malloc(sizeof(nor_flash_emulator_params));
        if (pflash->params != NULL)
        {
            memcpy(pflash->params, params, sizeof(nor_flash_emulator_params));
            pflash->buffer = malloc(sizeof(uint8_t) * pflash->params->flash_size);

            if (pflash->buffer != NULL)
            {
                if (((pflash->params->flash_size % pflash->params->sector_size) == 0) &&
                    ((pflash->params->sector_size % pflash->params->subsector_size) == 0) &&
                    ((pflash->params->subsector_size % pflash->params->page_size) == 0))
                {
                    pflash->number_of_sector = pflash->params->flash_size / pflash->params->sector_size;
                    pflash->number_of_subtector_per_sector = pflash->params->sector_size / pflash->params->subsector_size;
                    pflash->total_number_of_subsector = pflash->params->flash_size / pflash->params->subsector_size;
                    pflash->number_of_page_per_subsector = pflash->params->subsector_size / pflash->params->page_size;
                    pflash->total_number_of_page = pflash->params->flash_size / pflash->params->page_size;

                    memset(pflash->buffer, 0xFF, pflash->params->flash_size);
                    pflash->is_init = true;

                    status = 0;
                }
            }
        }
    }
    return status;
}

/**
 * @brief 
 * 
 * @param phandler 
 * @param file 
 * @return int 
 */
int nor_flash_emulator_save_to_file(nor_flash_emulator_handler *phandler, void *file)
{
    int status = -1;
    nor_flash_emulator_flash *pflash = (nor_flash_emulator_flash *)phandler->flash;

    if((phandler != NULL) && \
    (pflash != NULL) && \
    (file != NULL))
    {
        status = fwrite(pflash->buffer, sizeof(uint8_t), pflash->params->flash_size, file);
        if(status == pflash->params->flash_size)
        {
            status = 0;
        }
    }
    return status;
}
/**
 * @brief 
 * 
 * @param phandler 
 * @param sector_number 
 * @return int 
 */
int nor_flash_emulator_bulk_erase(nor_flash_emulator_handler *phandler)
{
    int i = 0;
    int status = -1;
    nor_flash_emulator_flash *pflash = (nor_flash_emulator_flash *)phandler->flash;

    if ((phandler != NULL) &&
        (pflash != NULL))
    {
        memset(pflash->buffer, 0xFF, pflash->params->flash_size);
        status = 0;
    }

    return status;
}

/**
 * @brief 
 * 
 * @param phandler 
 * @param subsector_number 
 * @return int 
 */
int nor_flash_emulator_erase_sector(nor_flash_emulator_handler *phandler, uint32_t sector_number)
{
    int i = 0;
    int status = -1;
    nor_flash_emulator_flash *pflash = (nor_flash_emulator_flash *)phandler->flash;

    if ((phandler != NULL) &&
        (pflash != NULL) &&
        (sector_number <= pflash->number_of_sector))
    {
        memset(pflash->buffer + (sector_number * pflash->params->sector_size), 0xFF, pflash->params->sector_size);
        status = 0;
    }

    return status;
}

/**
 * @brief 
 * 
 * @param phandler 
 * @param subsector_number 
 * @return int 
 */
int nor_flash_emulator_erase_subsector(nor_flash_emulator_handler *phandler, uint32_t subsector_number)
{
    int i = 0;
    int status = -1;
    nor_flash_emulator_flash *pflash = (nor_flash_emulator_flash *)phandler->flash;

    if ((phandler != NULL) &&
        (pflash != NULL) &&
        (subsector_number <= pflash->total_number_of_subsector))
    {
        memset(pflash->buffer + (subsector_number * pflash->params->subsector_size), 0xFF, pflash->params->subsector_size);
        status = 0;
    }

    return status;
}

/**
 * @brief 
 * 
 * @param phandler 
 * @param address 
 * @param len 
 * @param src 
 * @return int : number of bytes written
 */
int nor_flash_emulator_write(nor_flash_emulator_handler *phandler, uint32_t address, uint32_t len, uint8_t *src)
{
    int32_t i = 0;
    int status = -1;
    nor_flash_emulator_flash *pflash = (nor_flash_emulator_flash *)phandler->flash;

    if ((phandler != NULL) &&
        (pflash != NULL) &&
        (len != 0) &&
        (src != NULL))
    {
        if ((address + len) > phandler->params->flash_size)
        {
            len = (phandler->params->flash_size - address);
        }

        for (i = 0; i < len; i++)
        {
            *(pflash->buffer + address + i) &= src[i];
        }

        status = len;
    }

    return status;
}

/**
 * @brief 
 * 
 * @param address 
 * @param len 
 * @param dest 
 * @return int : number of bytes read
 */
int nor_flash_emulator_read(nor_flash_emulator_handler *phandler, uint32_t address, uint32_t len, uint8_t *dest)
{
    int status = -1;
    nor_flash_emulator_flash *pflash = (nor_flash_emulator_flash *)phandler->flash;

    if ((phandler != NULL) &&
        (pflash != NULL) &&
        (len != 0) &&
        (dest != NULL) &&
        (address <= pflash->params->flash_size))
    {
        if ((address + len) > phandler->params->flash_size)
        {
            len = (phandler->params->flash_size - address);
        }

        memcpy(dest, pflash->buffer + address, len);

        status = len;
    }

    return status;
}

/**
 * @brief 
 * 
 * @param phandler 
 */
void nor_flash_emulator_deinit(nor_flash_emulator_handler **phandler)
{
    nor_flash_emulator_handler *ph = *phandler;
    nor_flash_emulator_flash *pflash = NULL;

    if ((phandler != NULL) && (*phandler != NULL))
    {
        if (ph->params != NULL)
        {
            free(ph->params);
        }

        if (ph->flash != NULL)
        {
            pflash = (nor_flash_emulator_flash *)ph->flash;

            if (pflash->params != NULL)
            {
                free(pflash->params);
            }

            if (pflash->buffer != NULL)
            {
                free(pflash->buffer);
            }
        }

        free(*phandler);
        *phandler = NULL;
    }
}

/**
 * @brief 
 * 
 * @param params 
 * @return nor_flash_emulator_handler* 
 */
nor_flash_emulator_handler *nor_flash_emulator_init(nor_flash_emulator_params *params)
{
    nor_flash_emulator_handler *phandler = NULL;
    nor_flash_emulator_flash *pflash = NULL;
    int status = 0;

    if (params != NULL)
    {
        phandler = malloc(sizeof(nor_flash_emulator_handler));

        if (phandler != NULL)
        {
            phandler->params = malloc(sizeof(nor_flash_emulator_params));

            if (phandler->params != NULL)
            {
                memcpy(phandler->params, params, sizeof(nor_flash_emulator_params));
                phandler->flash = malloc(sizeof(nor_flash_emulator_flash));

                if (phandler->flash != NULL)
                {
                    pflash = (nor_flash_emulator_flash *)phandler->flash;
                    status = nor_flash_emulator_flash_init(pflash, phandler->params);

                    if (status == 0)
                    {
                        return phandler;
                    }
                }
            }
        }
    }

    nor_flash_emulator_deinit(&phandler);
    return NULL;
}