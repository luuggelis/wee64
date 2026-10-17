#include "include/ata.h"
#include "include/io.h"

#include <stddef.h>
#include <stdint.h>

#define ATA_DATA       0x1F0
#define ATA_SECCOUNT   0x1F2
#define ATA_LBA0       0x1F3
#define ATA_LBA1       0x1F4
#define ATA_LBA2       0x1F5
#define ATA_DRIVE      0x1F6
#define ATA_STATUS     0x1F7
#define ATA_COMMAND    0x1F7
#define ATA_ALTSTATUS  0x3F6

#define ATA_BSY        0x80
#define ATA_DRQ        0x08
#define ATA_DF         0x20
#define ATA_ERR        0x01

#define ATA_READ       0x20

#define ATA_TIMEOUT    1000000u

static int ata_wait(int requrie_rqd)
{
    for (uint32_t i = 0; i < ATA_TIMEOUT; i++)
    {
        uint8_t status = inb(ATA_STATUS);

        // no device or invalid status
        if (status == 0x00 || status == 0xFF)
            return -1;

        // drive is busy
        if (status & ATA_BSY)
            continue;

        // drive reports error
        if (status & (ATA_ERR | ATA_DF))
            return -1;

        // wait till data is ready if requested
        if (!requrie_rqd || (status & ATA_DRQ))
            return 0;
    }

    return -1;
}

static void ata_delay(void)
{
    for (int i = 0; i < 4; i++)
        (void)inb(ATA_ALTSTATUS);
}

int ata_read28(uint8_t drive, uint32_t lba, void *buffer)
{
    if (drive > 1 || lba >= 0x10000000u || buffer == NULL)
        return -1;

    outb(ATA_DRIVE,
         (uint8_t)(0xE0 | (drive << 4) |
                   ((lba >> 24) & 0x0F)));

    ata_delay();

    if (ata_wait(0) != 0)
        return -1;

    outb(ATA_SECCOUNT, 1);
    outb(ATA_LBA0, (uint8_t)lba);
    outb(ATA_LBA1, (uint8_t)(lba >> 8));
    outb(ATA_LBA2, (uint8_t)(lba >> 16));

    outb(ATA_COMMAND, ATA_READ);

    if (ata_wait(1) != 0)
        return -1;

    uint8_t *bytes = (uint8_t *)buffer;

    for (int i = 0; i < 256; i++) {
        uint16_t word = inw(ATA_DATA);

        bytes[i * 2]     = (uint8_t)(word & 0xFF);
        bytes[i * 2 + 1] = (uint8_t)(word >> 8);
    }

    return 0;
}
