#include "include/fat32.h"
#include "../kernel/include/ata.h"

#include <stdint.h>
#include <stddef.h>

static uint16_t read16(const uint8_t *p)
{
    return (uint16_t)p[0] |
           ((uint16_t)p[1] << 8);
}

static uint32_t read32(const uint8_t *p)
{
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

int fat32_init(fat32_fs_t *fs, uint8_t drive)
{
    uint8_t sector[FAT32_SECTOR_SIZE];

    if (fs == NULL)
        return -1;

    if (ata_read28(drive, 0, sector) != 0)
        return -1;

    if (sector[510] != 0x55 || sector[511] != 0xAA)
        return -1;
    
    fs->bytes_per_sector =
    read16(&sector[11]);

    fs->sectors_per_cluster =
        sector[13];

    fs->reserved_sectors =
        read16(&sector[14]);

    fs->fat_count =
        sector[16];

    fs->sectors_per_fat =
        read32(&sector[36]);

    fs->root_cluster =
        read32(&sector[44]);

    fs->fat_start =
        fs->reserved_sectors;

    fs->data_start =
        fs->reserved_sectors +
        ((uint32_t)fs->fat_count *
         fs->sectors_per_fat);

    return 0;
}