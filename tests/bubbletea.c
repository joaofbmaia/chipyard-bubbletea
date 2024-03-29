#include "bubbletea.h"
#include "mmio.h"
#include "bubbletea-lns.h"

int main(void) {
    // Test Data
    volatile unsigned char *test_data = (unsigned char *) 0x81000000;
    volatile unsigned char *test_result = (unsigned char *) 0x82000000;

    for (int i = 0; i < 64; i++) {
        test_data[i] = i;
    }

    for (int i = 0; i < 64; i++) {
        test_result[i] = 0;
    }

    //printf("Addr: %x\n", (uint64_t) &test_bitstream);
    // Write the bitstream base address
    reg_write64(BUBBLETEA_BITSTREAM_BASE_ADDR, (uint64_t) &load_no_store);
    // Start the bitstream loading
    reg_write8(BUBBLETEA_LOAD_BITSTREAM, 1);
    // Bitstream is being loaded
    while (reg_read8(BUBBLETEA_LOAD_BITSTREAM_DONE) == 0) ;
    // Bitstream is loaded

    // Start the accelerator
    reg_write8(BUBBLETEA_RUN, 1);
    // The accelerator is configuring itself with the bitstream
    while (reg_read8(BUBBLETEA_CONFIGURATION_DONE) == 0) ;
    // The accelerator is configured and will run
    // If we wanted, we could load a new bitstream now, while the accelerator is running
    // The accelerator is running
    while (reg_read8(BUBBLETEA_DONE) == 0) ;
    // The accelerator has finished running

    // Check the result
    for (int i = 0; i < 64; i++) {
        if (test_result[i] != i) {
            printf("Error: test_result[%d] = %d\n", i, test_result[i]);
            return 1;
        }
    }

    printf("Success\n");
    return 0;
}