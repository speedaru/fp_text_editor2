#pragma once
#include "core/terminal.hpp"


#ifdef SPD_PLATFORM_WINDOWS

spd::Terminal::Terminal() {
    m_hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    m_hIn = GetStdHandle(STD_INPUT_HANDLE);
    
    DWORD dwMode = 0;
    GetConsoleMode(m_hOut, &dwMode);
    m_originalOutMode = dwMode;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(m_hOut, dwMode);
}

spd::Terminal::~Terminal() {
    DisableRawMode();
    SetConsoleMode(m_hOut, m_originalOutMode);
}

void spd::Terminal::EnableRawMode() {
    DWORD dwMode = 0;
    GetConsoleMode(m_hIn, &dwMode);
    m_originalInMode = dwMode;
    dwMode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT);
    SetConsoleMode(m_hIn, dwMode);
}

void spd::Terminal::DisableRawMode() {
    SetConsoleMode(m_hIn, m_originalInMode);
}

spd::TermSize spd::Terminal::GetSize() const {
    TermSize size{};
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(m_hOut, &csbi);
    size.cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    size.rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    return size;
}

spd::KeyEvent spd::Terminal::ReadKey() const {
    DWORD read;
    INPUT_RECORD ir;
    while (true) {
        ReadConsoleInput(m_hIn, &ir, 1, &read);
        if (read && ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown) {
            auto& ke = ir.Event.KeyEvent;
            KeyEvent result = { Key::None, 0 };

            if (ke.uChar.AsciiChar >= 32 && ke.uChar.AsciiChar <= 126) {
                result.key = (Key)ke.uChar.AsciiChar;
                result.c = ke.uChar.AsciiChar;
                return result;
            }

            switch (ke.wVirtualKeyCode) {
                case VK_UP:     result.key = Key::ArrowUp;      break;
                case VK_DOWN:   result.key = Key::ArrowDown;    break;
                case VK_LEFT:   result.key = Key::ArrowLeft;    break;
                case VK_RIGHT:  result.key = Key::ArrowRight;   break;
                case VK_BACK:   result.key = Key::Backspace;    break;
                case VK_RETURN: result.key = Key::Enter;        break;
                case VK_DELETE: result.key = Key::Delete;       break;
                case VK_ESCAPE: result.key = Key::Escape;       break;
                case VK_HOME:   result.key = Key::Home;         break;
                case VK_END:    result.key = Key::End;          break;
                case 'S': if (ke.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) 
                             result.key = Key::Ctrl_S; break;
            }
            if (result.key != Key::None) return result;
        }
    }
}

#endif // SPD_PLATFORM_WINDOWS