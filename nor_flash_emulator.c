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

typedef struct nor_flash_emulator_flash_t
{
    uint8_t *buffer;

    nor_flash_emulator_params *params;

    uint32_t number_of_sector;
    uint32_t number_of_subtector_per_sector;
    uint32_t number_of_page_per_subsector;

    uint32_t total_number_of_subsector;
    uint32_t total_number_of_page;

} nor_flash_emulator_flash;

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

    if (params != NULL)
    {
        phandler = malloc(sizeof(nor_flash_emulator_handler));
        if (phandler != NULL)
        {
            phandler->params = malloc(sizeof(nor_flash_emulator_params));

            if (phandler->params != NULL)
            {
                memcpy(phandler->params, params, sizeof(nor_flash_emulator_params));

                if (phandler->params->flash_size != 0)
                {
                    phandler->flash = malloc(sizeof(nor_flash_emulator_flash));

                    if (phandler->flash != NULL)
                    {
                        pflash = (nor_flash_emulator_flash *)phandler->flash;
                        pflash->params = malloc(sizeof(nor_flash_emulator_params));
                        if (pflash->params != NULL)
                        {
                            memcpy(pflash->params, phandler->params, sizeof(nor_flash_emulator_params));
                            pflash->buffer = malloc(sizeof(uint8_t) * phandler->params->flash_size);

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
                                    return phandler;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (phandler != NULL)
    {
        if (phandler->params != NULL)
        {
            free(phandler->params);
        }

        if (phandler->flash != NULL)
        {
            pflash = phandler->flash;

            if (pflash->buffer != NULL)
            {
                free(pflash->buffer);
            }

            if (pflash->params != NULL)
            {
                free(pflash->params);
            }

            free(phandler->flash);
        }
        free(phandler);
    }

    return NULL;
}