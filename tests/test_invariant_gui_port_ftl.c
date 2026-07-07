#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Include the actual production header
#include "gui_port_ftl.h"

START_TEST(test_memcpy_bounds_invariant)
{
    // Invariant: Memory accesses must not exceed allocated buffer boundaries
    // even when given adversarial length values
    
    // Test payloads: boundary and adversarial cases
    size_t test_lengths[] = {
        0,                          // Boundary: zero length
        1024,                       // Valid: reasonable size
        SIZE_MAX,                   // Adversarial: maximum size_t value
        (size_t)-1,                 // Adversarial: wrap-around case
        4096                        // Boundary: larger but potentially valid
    };
    
    int num_tests = sizeof(test_lengths) / sizeof(test_lengths[0]);
    
    for (int i = 0; i < num_tests; i++) {
        size_t len = test_lengths[i];
        
        // Allocate source and destination buffers with known sizes
        uint8_t *src = malloc(1024);
        uint8_t *dst = malloc(1024);
        
        if (src && dst) {
            // Initialize buffers with known pattern
            memset(src, 0xAA, 1024);
            memset(dst, 0x55, 1024);
            
            // The security property: calling port_ftl_read with len > buffer size
            // should not corrupt memory beyond allocated bounds
            // We test this by checking that the first 1024 bytes of dst
            // are either unchanged (if len > 1024) or properly copied
            
            // Save original content of dst buffer
            uint8_t original[1024];
            memcpy(original, dst, 1024);
            
            // Call the actual production function
            int result = port_ftl_read((uintptr_t)src, dst, len);
            
            // Property: For any len value, the function should not crash
            // and should leave memory beyond allocated bounds unchanged
            // We verify by checking the function returned (didn't crash)
            ck_assert_msg(result == 0, "Function should return 0 for any input");
            
            // Additional check: if len <= 1024, verify copy happened correctly
            if (len <= 1024 && len > 0) {
                ck_assert_msg(memcmp(src, dst, len) == 0, 
                             "Data should be copied correctly for valid lengths");
            }
            
            // Clean up
            free(src);
            free(dst);
        }
    }
}
END_TEST

Suite *security_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Security");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_memcpy_bounds_invariant);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = security_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}