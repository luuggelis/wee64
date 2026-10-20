#ifndef ATA_H
#define ATA_H

#include <stdint.h>

int ata_read28(uint8_t drive, uint32_t lba, void *buffer);

int ata_write28(uint8_t drive, uint32_t lba, const void *buffer);

#endif