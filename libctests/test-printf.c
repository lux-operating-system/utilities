/*
 * Standard C Library Compliance Tests
 * Omar Elghoul, 2024
 */

#include <stdio.h>
#include <string.h>

struct integerTest {
    char *input;
    long param;
    char *output;
};

struct integerTest ints[] = {
    // positive decimal
    { "%d", 1, "1" },
    { "%d", 10, "10" },
    { "%d", 30, "30" },
    { "%2d", 1, " 1" },
    { "%4d", 8, "   8" },
    { "%04d", 8, "0008" },
    { "%05d", 1234567, "1234567" },
    { "%d", 88, "88"},

    // negative decimal
    { "%d", -1, "-1" },
    { "%d", -0, "0" },  // ensure there's no such thing as negative zero
    { "%04d", -5, "-005" },
    { "%04d", -1234, "-1234" },
    { "%5d", -8, "   -8" },
    { "%3d", -7890, "-7890" },

    // hexadecimal
    { "%x", 1, "1" },
    { "%x", 0x17, "17" },
    { "%x", 0xABC, "abc" },
    { "%2x", 5, " 5" },
    { "%6X", 0xBEEF, "  BEEF" },
    { "%6x", 0xBEEF, "  beef" },
    { "%08X", 0xCAFE, "0000CAFE" },
    { "%08x", 0xCAFEBABE, "cafebabe" },
    { "%03x", 0xA, "00a" },

    // positive longs
    { "%ld", 2, "2" },
    { "%ld", 1234, "1234" },
    { "%ld", 888, "888" },
    { "%04ld", 12, "0012" },
    { "%04ld", 1234, "1234" },
    { "%4ld", 44, "  44" },
    
    // negative longs
    { "%ld", -1, "-1" },
    { "%ld", -444, "-444" },
    { "%6ld", -1234, " -1234"},
};

int main(void) {
    char buf[32];

    int localPass = 0, localFail = 0;
    int totalPass = 0, totalFail = 0;

    printf("integer tests:\n");
    for(int i = 0; i < sizeof(ints) / sizeof(struct integerTest); i++) {
        sprintf(buf, ints[i].input, ints[i].param);
        if(strcmp(buf, ints[i].output)) {
            printf("\e[91m[fail]\e[0m ");
            localFail++;
            totalFail++;
        } else {
            printf("\e[92m[pass]\e[0m ");
            localPass++;
            totalPass++;
        }

        printf("printf(\"%s\", ...) - expected '%s', got '%s'\n", ints[i].input, ints[i].output, buf);
    }

    printf("\n");
    printf("total tests: %d, passed %d, failed %d\n", totalPass+totalFail, totalPass, totalFail);

    return totalFail;
}