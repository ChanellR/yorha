#include <asm/cpu_io.h>
#include <fs.h>
#include <interrupts.h>
#include <ata.h>
#include <util.h>
#include <io.h>


bool test_ata_pio(void) {

#define SECTOR_SIZE 512
#define SECTOR_COUNT 8 // size of a block

    uint8_t clear[SECTOR_SIZE * SECTOR_COUNT] = {0};
    ata_write_sectors(0, SECTOR_COUNT, clear);
    ata_read_sectors(0, SECTOR_COUNT, clear);

    for (int j = 0; j < 512 * SECTOR_COUNT; j++) {
        if (clear[j] != 0) {
            // output error
            kprintf("\n[clear] difference detected at byte % : ", j);
            kprintf("expected[%]=0", j);
            kprintf(" result[%]=", j);
            kputc(clear[j]);
            kprintf("\n");
            return false;
        }
    }

    uint8_t expected[SECTOR_SIZE * SECTOR_COUNT] = {0};
    const uint8_t result[SECTOR_SIZE * SECTOR_COUNT] = {0};

    for (int s = 0; s < SECTOR_COUNT; s++) {
        for (int i = 0; i < 512; i++) {
            expected[s * 512 + i] = (uint8_t)'a' + (i + s % 26); // shift by sector
        }
    }

    // writing 2 on both
    ata_write_sectors(0, SECTOR_COUNT, expected);
    ata_read_sectors(0, SECTOR_COUNT, result);

    for (int j = 0; j < 512 * SECTOR_COUNT; j++) {
        if (expected[j] != result[j]) {
            // output error
            kprintf("\ndifference detected at byte % : ", j);
            kprintf("expected[%]=", j);
            kputc(expected[j]);
            kprintf(" result[%]=", j);
            kputc(result[j]);
            kprintf("\n");
            return false;
        }
    }

#undef SECTOR_SIZE
#undef SECTOR_COUNT 

    return true;
}

bool test_intlen(void) {
    bool failing = false;
    failing |= intlen(10) != 2;
    failing |= intlen(-11234) != 6;
    failing |= intlen(0) != 1;
    return !failing;
}

bool test_strcmp(void) {
    bool passing = true;
    passing &= strcmp("abc", "ab") > 0;
    passing &= strcmp("abc", "abc") == 0;
    passing &= strcmp("", "") == 0;
    return passing;
}

bool test_apply_bitrange(void) {
    bool passing = true;
    uint32_t bitmap[16] = {0};
    BitRange range = {.start = 30, .length = 2};
    apply_bitrange(bitmap, range, true);
    passing &= bitmap[0] == 0x00000003;
    // kprintf("bitmap[0]=%x\n", bitmap[0]);
    range.start = 0;
    range.length = 1;
    apply_bitrange(bitmap, range, true);
    passing &= bitmap[0] == 0x80000003;
    // kprintf("bitmap[0]=0x%x\n", bitmap[0]);
    range.start = 63;
    range.length = 3;
    apply_bitrange(bitmap, range, true);
    // kprintf("bitmap[1]=0x%x bitmap[2]=0x%x \n", bitmap[1], bitmap[2]);
    passing &= bitmap[1] == 0x00000001 && bitmap[2] == 0xC0000000;
    range.start = 28;
    range.length = 8;
    apply_bitrange(bitmap, range, true);
    // kprintf("bitmap[0]=0x%x bitmap[1]=0x%x \n", bitmap[0], bitmap[1]);
    passing &= bitmap[0] == 0x8000000F && bitmap[1] == 0xF0000001;
    apply_bitrange(bitmap, range, false);
    // kprintf("bitmap[0]=0x%x bitmap[1]=0x%x \n", bitmap[0], bitmap[1]);
    passing &= bitmap[0] == 0x80000000 && bitmap[1] == 0x00000001;

    return passing;
}

bool test_alloc_bitrange() {
    bool passing = true;
    uint32_t bitmap[2] = {0};
    BitRange result;
    result = alloc_bitrange(bitmap, 2);
    passing &= result.start == 0 && result.length == 2;
    // kprintf("result = {.start = %u, .length = %u}\n", result.start, result.length);
    result = alloc_bitrange(bitmap, 8);
    // kprintf("result = {.start = %u, .length = %u}\n", result.start, result.length);
    passing &= result.start == 2 && result.length == 8;
    result = alloc_bitrange(bitmap, 32);
    // kprintf("result = {.start = %u, .length = %u}\n", result.start, result.length);
    // kprintf("bitmap = %b %b\n", bitmap[0], bitmap[1]);
    passing &= result.start == 10 && result.length == 32;
    
    // dealloc
    result.start = 2;
    result.length = 8;
    dealloc_bitrange(bitmap, result);
    // kprintf("bitmap = %b %b\n", bitmap[0], bitmap[1]);
    result = alloc_bitrange(bitmap, 6);
    // kprintf("result = {.start = %u, .length = %u}\n", result.start, result.length);
    passing &= result.start == 2 && result.length == 6;
    // kprintf("bitmap = %b %b\n", bitmap[0], bitmap[1]);

    return passing;
}

void run_tests(void) {
    kprintf("Running Tests...\n");
    
    // kprintf("test_ata_pio...");
    // kprintf((test_ata_pio()) ? "OK\n" : "FAIL\n");

    kprintf("test_intlen...");
    kprintf((test_intlen()) ? "OK\n" : "FAIL\n");
    
    kprintf("test_strcmp...");
    kprintf((test_strcmp()) ? "OK\n" : "FAIL\n");

    kprintf("test_apply_bitrange...");
    kprintf((test_apply_bitrange()) ? "OK\n" : "FAIL\n");

    kprintf("test_alloc_bitrange...");
    kprintf((test_alloc_bitrange()) ? "OK\n" : "FAIL\n");
    
    kprintf("\n");
}
        