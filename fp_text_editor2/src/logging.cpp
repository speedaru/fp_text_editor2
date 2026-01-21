#include "logging.h"

static std::ofstream logFile{};

void logging::LoggerInit(const char* filename) {
    logFile = std::ofstream(filename, std::ios::binary);
    if (!logFile) {
        fprintf(stderr, "failed to open file '%s'\n", filename);
        return;
    }
}

void logging::LoggerShutdown() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void logging::LogOutput(LogLevel level, const char* file, int line, const char* func, const char* fmt, ...) {
    if (!logFile) return;

    const char* level_strings[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR"};
    
    // Get current time
    time_t now = time(NULL);
    tm t{};
    localtime_s(&t, &now);
    
    // Print Header: [TIME] [LEVEL] [FILE:LINE in FUNC]
	logFile << std::format(
		"[{:02}:{:02}:{:02}] [{}] [{}:{} in {}]: ",
		t.tm_hour, t.tm_min, t.tm_sec,
		level_strings[level], file, line, func
	);

    // Print actual message
	va_list args;
	va_start(args, fmt);
    int size = vsnprintf(nullptr, 0, fmt, args);

    char* buff = (char*)malloc(size + 1);
    if (!buff) {
        return;
    }

    vsnprintf(buff, (size_t)size + 1, fmt, args);
	va_end(args);

    logFile.write(buff, size);

    free(buff);
	logFile.flush();
}