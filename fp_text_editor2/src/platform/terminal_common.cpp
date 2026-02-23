#include "core/terminal.hpp"


void spd::Terminal::Clear() {
    //Write(ANSI_CLEAR_SCREEN ANSI_SET_CURSOR_HOME);
#ifdef SPD_PLATFORM_WINDOWS
    TermSize size = GetSize();
    DWORD consoleSize = size.rows * size.cols;
    COORD topLeft{ 0, 0 };
    
    DWORD written;
    FillConsoleOutputCharacter(m_hOut, L' ', consoleSize, topLeft, &written);
    SetConsoleCursorPosition(m_hOut, topLeft);
#else
    system("clear")
#endif
}

void spd::Terminal::SetCursorPos(int row, int col) {
    char buf[32]{ 0 };
#ifdef SPD_PLATFORM_WINDOWS
    sprintf_s(buf, ANSI_SET_CURSOR_POS, row + 1, col + 1);
#else
    sprintf(buf, ANSI_SET_CURSOR_POS, row + 1, col + 1);
#endif
    Write(buf);
}

void spd::Terminal::HideCursor() {
    Write(ANSI_HIDE_CURSOR);
}

void spd::Terminal::ShowCursor() {
    Write(ANSI_SHOW_CURSOR);
}

size_t spd::Terminal::Write(const char* str) {
    return fwrite(str, 1, spd::strlen(str), stdout);
}

size_t spd::Terminal::Write(const char* buff, size_t len) {
    if (len > 0) {
		return fwrite(buff, 1, len, stdout);
    }
    return 0ull;
}

void spd::Terminal::Flush() {
    fflush(stdout);
}