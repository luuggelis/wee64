#include "include/fat32.h"
#include "../kernel/include/ata.h"

#include <stdint.h>
#include <stddef.h>

extern void print(const char *str);

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

    fs->drive = drive;

    if (fs == NULL)
        return -1;

    if (ata_read28(fs->drive, 0, sector) != 0)
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

int fat32_read_cluster(
    fat32_fs_t *fs,
    uint32_t cluster,
    void *buffer,
    size_t buffer_size)
{
    if (fs == NULL || buffer == NULL)
        return -1;

    if (cluster < 2)
        return -1;

    size_t cluster_size =
        (size_t)fs->bytes_per_sector *
        fs->sectors_per_cluster;

    if (buffer_size < cluster_size)
        return -1;

    uint32_t first_sector =
        fs->data_start +
        (cluster - 2) * fs->sectors_per_cluster;

    uint8_t *dest = (uint8_t *)buffer;

    for (uint32_t i = 0;
         i < fs->sectors_per_cluster;
         i++)
    {
        if (ata_read28(
                fs->drive,
                first_sector + i,
                dest + i * fs->bytes_per_sector) != 0)
        {
            return -1;
        }
    }

    return 0;
}

int fat32_list_root(fat32_fs_t *fs)
{
    uint8_t buffer[4096];

    if (fat32_read_cluster(fs, fs->root_cluster, buffer, sizeof(buffer)) != 0)
        return -1;

    for (uint32_t offset = 0;
         offset < fs->bytes_per_sector * fs->sectors_per_cluster;
         offset += 32)
    {
        uint8_t *entry = &buffer[offset];

        if (entry[0] == 0x00)
            break;

        if (entry[0] == 0xE5)
            continue;

        if (entry[11] == 0x0F)
            continue;

        if (entry[11] & 0x08)
            continue;

        if (entry[11] & 0x10)
            print("[DIR] ");

        for (int i = 0; i < 11; i++)
        {
            char c = (char)entry[i];

            if (i == 8)
                print(".");

            if (c == ' ')
                continue;

            char str[2] = { c, '\0' };
            print(str);
        }

        print("\n");
    }

    return 0;
}