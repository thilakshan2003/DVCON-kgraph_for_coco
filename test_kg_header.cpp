// Test harness for bare-metal KG lookup
// Compile: g++ -std=c++17 -fno-exceptions -fno-rtti -O2 -Wall -Wextra test_kg_header.cpp -o test_kg
#include <cstdio>
#include <cstring>

// Include our KG lookup header
#include "kg_lookup.h"

void print_result(const char *prompt, const KgResult &res)
{
    printf("Prompt: '%s'\n", prompt);
    if (res.matched)
    {
        printf("  ✓ Matched! Found %d classes: ", res.count);
        for (uint8_t i = 0; i < res.count; ++i)
        {
            printf("%s", COCO_CLASS_NAMES[res.classes[i]]);
            if (i < res.count - 1)
                printf(", ");
        }
        printf("\n");
    }
    else
    {
        printf("  ✗ No match\n");
    }
}

int main()
{
    printf("=== KG Bare-Metal Lookup Test ===\n\n");

    // Test 1: simple "drink"
    {
        const char prompt1[] = "drink water";
        KgResult res = kg_lookup(prompt1, strlen(prompt1));
        print_result(prompt1, res);
    }

    // Test 2: synonym "sip" (should alias to drink)
    {
        const char prompt2[] = "sip some tea";
        KgResult res = kg_lookup(prompt2, strlen(prompt2));
        print_result(prompt2, res);
    }

    // Test 3: "eat" action
    {
        const char prompt3[] = "eat apple";
        KgResult res = kg_lookup(prompt3, strlen(prompt3));
        print_result(prompt3, res);
    }

    // Test 4: "pet" action
    {
        const char prompt4[] = "pet the dog";
        KgResult res = kg_lookup(prompt4, strlen(prompt4));
        print_result(prompt4, res);
    }

    // Test 5: alias "bark" (should map to pet)
    {
        const char prompt5[] = "bark loudly";
        KgResult res = kg_lookup(prompt5, strlen(prompt5));
        print_result(prompt5, res);
    }

    // Test 6: no match
    {
        const char prompt6[] = "xyzabc notaword";
        KgResult res = kg_lookup(prompt6, strlen(prompt6));
        print_result(prompt6, res);
    }

    // Test 7: uppercase conversion
    {
        const char prompt7[] = "DRINK COFFEE";
        KgResult res = kg_lookup(prompt7, strlen(prompt7));
        print_result(prompt7, res);
    }

    // Test 8: long prompt truncation
    {
        const char prompt8[] = "this is a very long prompt that should be truncated at one hundred characters which is the default maximum so lets see if it truncates properly";
        KgResult res = kg_lookup(prompt8, strlen(prompt8));
        print_result(prompt8, res);
    }

    // Test 9: punctuation stripping
    {
        const char prompt9[] = "drink... coffee!!!";
        KgResult res = kg_lookup(prompt9, strlen(prompt9));
        print_result(prompt9, res);
    }

    printf("\n=== All tests completed ===\n");
    printf("NUM_COCO_CLASSES = %d\n", NUM_COCO_CLASSES);
    printf("KG_LOOKUP_TABLE_SIZE = %d\n", KG_LOOKUP_TABLE_SIZE);

    return 0;
}
