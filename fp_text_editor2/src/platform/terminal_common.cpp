#include "core/terminal.hpp"


void spd::Terminal::Clear() {
    Write(ANSI_CLEAR_SCREEN ANSI_SET_CURSOR_HOME);
}

void spd::Terminal::SetCursorPos(int row, int col) {
    char buf[32]{ 0 };
    sprintf(buf, ANSI_SET_CURSOR_POS, row + 1, col + 1);
    Write(buf);
}

void spd::Terminal::HideCursor() {
    Write(ANSI_HIDE_CURSOR);
}

void spd::Terminal::ShowCursor() {
    Write(ANSI_SHOW_CURSOR);
}

void spd::Terminal::Write(const char* str) {
    fwrite(str, 1, strlen(str), stdout);
}

void spd::Terminal::Write(const char* buff, size_t len) {
    fwrite(buff, 1, len, stdout);
}

void spd::Terminal::Flush() {
    fflush(stdout);
}