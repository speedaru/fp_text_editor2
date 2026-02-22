#include "terminal.hpp"
#include <cstdio>

#ifdef SPD_PLATFORM_LINUX
#include <sys/ioctl.h>
#include <termios.h>
#endif


spd::Terminal::Terminal() {
#ifdef SPD_PLATFORM_WINDOWS
    m_hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    m_hIn = GetStdHandle(STD_INPUT_HANDLE);
    
    // Enable ANSI support on modern Windows
    DWORD dwMode = 0;
    GetConsoleMode(m_hOut, &dwMode);
    m_originalOutMode = dwMode;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(m_hOut, dwMode);
#else
    tcgetattr(STDIN_FILENO, &m_originalTermios);
#endif
}

spd::Terminal::~Terminal() {
    DisableRawMode();
#ifdef SPD_PLATFORM_WINDOWS
    SetConsoleMode(m_hOut, m_originalOutMode);
#endif
}

void spd::Terminal::Clear() {
    // ANSI magic to clear screen and home cursor
    Write("\x1b[2J\x1b[H");
}

void spd::Terminal::SetCursorPos(int row, int col) {
    char buf[32];
    // ANSI: \x1b[row;colH (1-based indexing)
#ifdef SPD_PLATFORM_WINDOWS
    sprintf_s(buf, "\x1b[%d;%dH", row + 1, col + 1);
#else
    sprintf(buf, "\x1b[%d;%dH", row + 1, col + 1);
#endif
    Write(buf);
}

void spd::Terminal::HideCursor() {
    Write("\x1b[?25l");
}

void spd::Terminal::ShowCursor() {
    Write("\x1b[?25h");
}

spd::TermSize spd::Terminal::GetSize() const {
    TermSize size{};
#ifdef SPD_PLATFORM_WINDOWS
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(m_hOut, &csbi);
    size.cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    size.rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    size.cols = ws.ws_col;
    size.rows = ws.ws_row;
#endif
    return size;
}

void spd::Terminal::EnableRawMode() {
#ifdef SPD_PLATFORM_WINDOWS
    DWORD dwMode = 0;
    GetConsoleMode(m_hIn, &dwMode);
    m_originalInMode = dwMode;
    // Disable Echo, Line Input (Enter to send), and Signal keys (Ctrl+C)
    dwMode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT);
    SetConsoleMode(m_hIn, dwMode);
#else
    struct termios raw = m_originalTermios;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
#endif
}

void spd::Terminal::DisableRawMode() {
#ifdef SPD_PLATFORM_WINDOWS
    SetConsoleMode(m_hIn, m_originalInMode);
#else
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &m_originalTermios);
#endif
}

#ifdef SPD_PLATFORM_WINDOWS
spd::KeyEvent spd::Terminal::ReadKey() const {
    DWORD read;
    INPUT_RECORD ir;
    while (true) {
        ReadConsoleInput(m_hIn, &ir, 1, &read);
        if (read && ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown) {
            auto& ke = ir.Event.KeyEvent;
            KeyEvent result = { Key::None, 0 };

            // handle ascii characters
            if (ke.uChar.AsciiChar >= 32 && ke.uChar.AsciiChar <= 126) {
                result.key = (Key)ke.uChar.AsciiChar;
                result.c = ke.uChar.AsciiChar;
                return result;
            }

            // handle special keys
            switch (ke.wVirtualKeyCode) {
                case VK_UP:     result.key = Key::ArrowUp;      break;
                case VK_DOWN:   result.key = Key::ArrowDown;    break;
                case VK_LEFT:   result.key = Key::ArrowLeft;    break;
                case VK_RIGHT:  result.key = Key::ArrowRight;   break;
                case VK_BACK:   result.key = Key::Backspace;    break;
                case VK_RETURN: result.key = Key::Enter;        break;
                case VK_DELETE: result.key = Key::Delete;       break;
                case VK_ESCAPE: result.key = Key::Escape;       break;
                case VK_PRIOR:  result.key = Key::PageUp;       break;
                case VK_NEXT:   result.key = Key::PageDown;     break;
                case VK_HOME:   result.key = Key::Home;         break;
                case VK_END:    result.key = Key::End;          break;
                case 'S': if (ke.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) 
                             result.key = Key::Ctrl_S; break;
            }
            if (result.key != Key::None) return result;
        }
    }
}
#endif

#ifdef SPD_PLATFORM_LINUX
spd::KeyEvent spd::Terminal::ReadKey() {
    char c;
    while (read(STDIN_FILENO, &c, 1) != 1);

    KeyEvent result = { Key::None, 0 };

    if (c == '\x1b') { // Escape sequence detected
        char seq[3];
        // Non-blocking read to see if more bytes follow ESC
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return { Key::Escape, 0 };
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return { Key::Escape, 0 };

        if (seq[0] == '[') {
            switch (seq[1]) {
            case 'A': return { Key::ArrowUp, 0 };
            case 'B': return { Key::ArrowDown, 0 };
            case 'C': return { Key::ArrowRight, 0 };
            case 'D': return { Key::ArrowLeft, 0 };
            case '3': return { Key::Delete, 0 }; // Sequence is \x1b[3~
            }
        }
        return { Key::Escape, 0 };
    }
    else if (c == 127) return { Key::Backspace, 0 };
    else if (c == '\r' || c == '\n') return { Key::Enter, 0 };
    else if (c == ('s' & 0x1f)) return { Key::Ctrl_S, 0 }; // Ctrl shortcut logic

    // Default printable char
    result.key = (Key)c;
    result.c = c;
    return result;
}
#endif

void spd::Terminal::Write(const char* str) {
    fwrite(str, sizeof(char), strlen(str), stdout);
}

void spd::Terminal::Write(const char* buff, size_t len) {
    fwrite(buff, sizeof(char), len, stdout);
}

void spd::Terminal::Flush() {
    fflush(stdout);
}
