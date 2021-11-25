/**
 * @file nor_flash_emulator.h
 * @author Julien Lespagnol (lespagnolj@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-11-11
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __NOR_FLASH_EMULATOR_H_
#define __NOR_FLASH_EMULATOR_H_
#include <stdint.h>

/**
 * @brief 
 * 
 */
typedef struct nor_flash_emulator_params_t
{
    uint32_t flash_size;
    uint32_t sector_size;
    uint32_t subsector_size;
    uint32_t page_size;
}nor_flash_emulator_params;

/**
 * @brief 
 * 
 */
typedef struct nor_flash_emulator_handler_t
{
    nor_flash_emulator_params *params;
    void *flash;
}nor_flash_emulator_handler;


int nor_flash_emulator_read(nor_flash_emulator_handler *phandler, uint32_t address, uint32_t len, uint8_t *dest);

/**
 * @brief 
 * 
 * @param phandler 
 */
void nor_flash_emulator_deinit(nor_flash_emulator_handler **phandler);

/**
 * @brief 
 * 
 * @param params 
 * @return nor_flash_emulator_handler* 
 */
nor_flash_emulator_handler * nor_flash_emulator_init(nor_flash_emulator_params *params);
#endif