#include "logging.h"
#include <stdarg.h>

static FILE* logFile = NULL;

void logging::LoggerInit(const char* filename) {
    logFile = fopen(filename, "w");
    if (!logFile) {
        fprintf(stderr, "Failed to open log file!\n");
    }
}

void logging::LoggerShutdown() {
    if (logFile) {
        fclose(logFile);
    }
}

void logging::LogOutput(LogLevel level, const char* file, int line, const char* func, const char* fmt, ...) {
    if (!logFile) return;

    const char* level_strings[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR"};
    
    // Get current time
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    
    // Print Header: [TIME] [LEVEL] [FILE:LINE in FUNC]
    fprintf(logFile, "[%02d:%02d:%02d] [%s] [%s:%d in %s]: ", 
            t->tm_hour, t->tm_min, t->tm_sec, 
            level_strings[level], file, line, func);

    // Print actual message
    va_list args;
    va_start(args, fmt);
    vfprintf(logFile, fmt, args);
    va_end(args);

    fprintf(logFile, "\n");
    fflush(logFile); // Ensure it's written even if we crash next line
}