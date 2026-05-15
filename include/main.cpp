#include <iostream>
#include <chrono>
#include <cstring>
#include "kg_lookup.h"
// #include <windows.h>   // Added for Windows API
// #include <psapi.h> // Added for Memory Stats

int main()
{
    // 1. The test prompt
    const char *prompt = "I need to sip some water";
    uint16_t len = std::strlen(prompt);

    // 2. Start the high-resolution hardware clock
    auto start = std::chrono::high_resolution_clock::now();

    // 3. Execute your bare-metal lookup
    KgResult result = kg_lookup(prompt, len);

    // 4. Stop the clock
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    // 5. Print the results
    std::cout << "--- FPGA Semantic Lookup Test ---" << std::endl;
    std::cout << "Prompt: '" << prompt << "'" << std::endl;

    if (result.matched)
    {
        std::cout << "Target Classes Found: " << (int)result.count << std::endl;
        for (int i = 0; i < result.count; ++i)
        {
            std::cout << "  - " << result.classes[i] << std::endl;
        }
    }
    else
    {
        std::cout << "Target Classes Found: 0 (No Match)" << std::endl;
    }

    std::cout << "---------------------------------" << std::endl;
    std::cout << "Execution Time: " << duration.count() << " nanoseconds" << std::endl;

    // PROCESS_MEMORY_COUNTERS_EX pmc;
    // GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS *)&pmc, sizeof(pmc));

    // // PrivateUsage is the actual RAM your program requested from the OS
    // SIZE_T peakRAM = pmc.PrivateUsage;

    // std::cout << "Peak RAM Usage: " << peakRAM << " bytes (" << (peakRAM / 1024) << " KB)" << std::endl;
    return 0;
}