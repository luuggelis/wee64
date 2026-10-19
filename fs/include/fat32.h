#ifndef FAT32_H
#define FAT32_H

#include <stdint.h>
#include <stddef.h>

#define FAT32_SECTOR_SIZE 512

typedef struct
{
    uint8_t  drive;

    uint16_t bytes_per_sector;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t  fat_count;
    uint32_t sectors_per_fat;
    uint32_t root_cluster;

    uint32_t fat_start;
    uint32_t data_start;
} fat32_fs_t;

int fat32_init(fat32_fs_t *fs, uint8_t drive);

int fat32_read_cluster(
    fat32_fs_t *fs,
    uint32_t cluster,
    void *buffer,
    size_t buffer_size
);

int fat32_list_root(fat32_fs_t *fs);

#endif
