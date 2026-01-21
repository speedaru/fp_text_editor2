#pragma once

// platform detection
#if defined(_WIN32) || defined(_WIN64)
    #define SPD_PLATFORM_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else
    #define SPD_PLATFORM_LINUX
    #include <unistd.h>
#endif


#define __RELATIVE_FILE__ __FILE__ + sizeof(PROJECT_DIR) - 1
