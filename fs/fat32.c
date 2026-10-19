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

    if (fs == NULL)
        return -1;
    
    fs->drive = drive;

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

    print("[ fat32 ] reading root directory...\n");

    if (fat32_read_cluster(
            fs,
            fs->root_cluster,
            buffer,
            sizeof(buffer)) != 0)
    {
        print("[ fat32 ] failed to read root directory\n");
        return -1;
    }

    print("[ fat32 ] root directory read OK\n");

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


static int fat32_next_cluster(fat32_fs_t *fs,
                              uint32_t cluster,
                              uint32_t *next)
{
    if (fs == NULL || next == NULL || cluster < 2)
        return -1;

    uint32_t fat_offset = cluster * 4;
    uint32_t sector_num =
        fs->fat_start + fat_offset / fs->bytes_per_sector;
    uint32_t offset = fat_offset % fs->bytes_per_sector;

    uint8_t sector[FAT32_SECTOR_SIZE];

    if (ata_read28(fs->drive, sector_num, sector) != 0)
        return -1;

    *next = read32(&sector[offset]) & 0x0FFFFFFF;

    if (*next >= 0x0FFFFFF8)
        return 1;

    if (*next < 2 || *next == 0x0FFFFFF7)
        return -1;

    return 0;
}

int fat32_read_file(fat32_fs_t *fs,
                    uint32_t first_cluster,
                    uint32_t file_size,
                    void *buffer,
                    size_t buffer_size)
{
    if (fs == NULL || buffer == NULL || first_cluster < 2)
        return -1;

    if (file_size > buffer_size)
        return -1;

    if (fs->bytes_per_sector != FAT32_SECTOR_SIZE ||
        fs->sectors_per_cluster == 0)
        return -1;

    uint32_t cluster = first_cluster;
    uint8_t *dest = (uint8_t *)buffer;
    uint32_t remaining = file_size;

    while (remaining > 0)
    {
        uint8_t cluster_buffer[4096];

        size_t cluster_size =
            (size_t)fs->bytes_per_sector *
            fs->sectors_per_cluster;

        if (cluster_size > sizeof(cluster_buffer))
            return -1;

        if (fat32_read_cluster(fs, cluster,
                               cluster_buffer,
                               sizeof(cluster_buffer)) != 0)
            return -1;

        size_t amount = remaining;
        if (amount > cluster_size)
            amount = cluster_size;

        for (size_t i = 0; i < amount; i++)
            dest[i] = cluster_buffer[i];

        dest += amount;
        remaining -= amount;

        if (remaining == 0)
            break;

        uint32_t next;
        int rc = fat32_next_cluster(fs, cluster, &next);

        if (rc != 0)
            return -1;

        cluster = next;
    }

    return 0;
}
