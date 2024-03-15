#include "bubbletea.h"
#include "mmio.h"
#include "matmul_mini2x2_bitstream.h"

#define NR 32
#define NI 2
#define NJ 2
#define NK 2

int main(void) {
    // Test Data
    volatile unsigned char *a = (unsigned char *) 0x81000000;
    volatile unsigned char *b = (unsigned char *) 0x82000000;
    volatile unsigned char *c = (unsigned char *) 0x83000000;
    volatile unsigned char *d = (unsigned char *) 0x84000000;

    // C = A * B + D

    for (int r = 0; r < NR; r++) {
        for (int i = 0; i < NI; i++) {
            for (int k = 0; k < NK; k++) {
                a[r*NI + i*NI + k] = r*NI + i*NI + k;
            }
        }
    }

    for (int r = 0; r < NR; r++) {
        for (int k = 0; k < NK; k++) {
            for (int j = 0; j < NJ; j++) {
                b[r*NK + k*NK + j] = r*NK + k*NK + j;
            }
        }
    }

    for (int r = 0; r < NR; r++) {
        for (int i = 0; i < NI; i++) {
            for (int j = 0; j < NJ; j++) {
                c[r*NI + i*NI + j] = 0;
            }
        }
    }

    for (int r = 0; r < NR; r++) {
        for (int i = 0; i < NI; i++) {
            for (int j = 0; j < NJ; j++) {
                d[r*NI + i*NI + j] = 2*r*NI + 2*i*NI + 2*j;
            }
        }
    }

    //printf("Addr: %x\n", (uint64_t) &test_bitstream);
    // Write the bitstream base address
    reg_write64(BUBBLETEA_BITSTREAM_BASE_ADDR, (uint64_t) &matmul_mini2x2_bitstream);
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
    // for (int i = 0; i < 64; i++) {
    //     if (test_result[i] != i) {
    //         printf("Error: test_result[%d] = %d\n", i, test_result[i]);
    //         return 1;
    //     }
    // }

    // printf("Success\n");
    return 0;
}